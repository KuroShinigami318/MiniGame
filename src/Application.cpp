#include "stdafx.h"
#include "Application.h"
#include "IHeartBeats.h"
#include "IInputDevice.h"
#include "Game.h"
#include "Log.h"
#include "MacrosUtils.h"

#include "details/platforms.h"
#if defined(USE_POSIX_API)
#include <sys/ioctl.h>
#include <unistd.h>
#endif

Application::Application(utils::unique_ref<utils::IHeartBeats> i_heart)
    : m_isExiting(false), m_exitReason(ExitReason::_COUNT)
    , m_multiplier(1)
    , m_actualElapsed(0)
    , m_heart(std::move(i_heart))
    , m_frameThread({ &Application::FramePrologue, this }, { &Application::FrameEpilogue, this })
{
    utils::async(GetNextFrameMessageQueue(), &Application::CreateGame, this);
    utils::Log::RegisterWriter<utils::Log::DefaultConsoleWriter>();
}

Application::~Application() = default;

utils::MessageSink_mt& Application::GetNextFrameMessageQueue()
{
    return m_frameThread.GetNextFrameMessageQueue();
}

utils::MessageSink& Application::GetThisFrameMessageQueue()
{
    return m_frameThread.GetFrameMessageQueue();
}

utils::IRecursiveControl& Application::GetRecursiveControl()
{
    return m_frameThread;
}

void Application::Execute()
{
    utils::steady_clock::time_point beginFrameTimePoint = utils::steady_clock::now();
    utils::steady_clock::time_point lastFrameTimePoint = utils::steady_clock::now();
    START:
    while (!m_isExiting)
    {
        m_actualElapsed = utils::steady_clock::now() - beginFrameTimePoint;
        beginFrameTimePoint = utils::steady_clock::now();
        m_frameThread.Submit(GetElapsedSeconds() * m_multiplier);
        m_mainQueue.dispatch();
        {
            Render();
        }
        m_frameResult = m_frameThread.Wait();
        m_actualElapsed = utils::steady_clock::now() - beginFrameTimePoint;
        auto remaining = m_heart->cast_to_duration() - m_actualElapsed;
        lastFrameTimePoint += utils::IsInBounds(remaining.count(), 0., std::numeric_limits<double>::infinity()) ? m_actualElapsed + std::chrono::duration_cast<utils::nanosecs>(remaining) : m_actualElapsed;
        std::unique_lock lk(m_mutex);
        m_cv.wait_until(lk, lastFrameTimePoint, [this]() {return m_isExiting; });
    }
    if (m_optExitCallback.has_value())
    {
        utils::async(GetNextFrameMessageQueue(), *m_optExitCallback);
    }
    if (m_exitReason == ExitReason::Reload)
    {
        RequestExit(ExitReason::_COUNT);
        goto START;
    }
	SyncWithFrameThread();
    utils::async(GetNextFrameMessageQueue(), &Application::DestroyGame, this);
	SyncWithFrameThread();
}

void Application::RequestExit(ExitReason i_reason)
{
    m_isExiting = i_reason != ExitReason::_COUNT;
    m_exitReason = i_reason;
    m_optExitCallback.reset();
    switch (i_reason)
    {
    case ExitReason::Reload:
    {
        m_optExitCallback.emplace(&Game::OnReload, m_game.get());
    }
    break;
    case ExitReason::Exit:
    {
        m_optExitCallback.emplace(&Game::OnExit, m_game.get());
    }
    break;
    }
}

const bool Application::IsExiting() const
{
    return m_isExiting;
}

float Application::GetElapsedSeconds() const
{
    return std::chrono::duration_cast<utils::duration<float>>(m_actualElapsed).count();
}

void Application::RegisterInputDevice(IInputDevice& i_inputDevice)
{
    if (!m_game)
    {
        utils::async(m_mainQueue, &Application::RegisterInputDevice, this, i_inputDevice);
    }
    m_connections.push_back(i_inputDevice.sig_onQuit.Connect(&Application::RequestExit, this, ExitReason::Exit));
    NOT_RELEASE(m_connections.push_back(i_inputDevice.sig_onInput.Connect(
        [this](std::string input)
        {
            std::transform(input.begin(), input.end(), input.begin(), [](unsigned char c) { return std::tolower(c); });
            if (input.find("r") != std::string::npos)
            {
                RequestExit(ExitReason::Reload);
                m_multiplier = 1.f;
            }
            else if (input.find("+") != std::string::npos)
            {
               m_multiplier *= 2.f;
            }
            else if (input.find("-") != std::string::npos)
            {
               m_multiplier *= 0.5f;
            }
            return true;
        }));)
    if (m_game)
    {
        utils::async(GetNextFrameMessageQueue(), &Game::RegisterInputDevice, m_game.get(), i_inputDevice);
    }
}
void Application::UnregisterInputDevice(IInputDevice& i_inputDevice)
{
    m_connections.clear();
    if (m_game)
    {
        utils::async(GetNextFrameMessageQueue(), &Game::UnregisterInputDevice, m_game.get(), i_inputDevice);
        m_frameThread.Submit(GetElapsedSeconds());
		m_frameThread.Wait();
    }
}

void Application::FramePrologue(float i_elapsed)
{
    if (m_game)
    {
        m_game->FramePrologue(i_elapsed);
    }
}

Application::FrameResult Application::FrameEpilogue() const
{
    FrameResult frameResult{.renderStream = std::make_unique<std::stringstream>()};
    if (m_game)
    {
        m_game->FrameEpilogue(*frameResult.renderStream);
    }
    return frameResult;
}

void Application::CreateGame()
{
    m_game = std::make_unique<Game>(RequestExitCallbackT(&Application::RequestExit, this), GetNextFrameMessageQueue(), GetThisFrameMessageQueue(), GetRecursiveControl());
}

void Application::DestroyGame()
{
    m_connections.clear();
    m_game.reset();
}

namespace
{
// Query the live terminal size. Returns false (leaving the outputs untouched) when the
// size cannot be determined, e.g. when output is redirected to a file or pipe.
bool QueryTerminalSize(int& o_width, int& o_height)
{
#if defined(USE_WIN32_API)
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
    {
        o_width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        o_height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
        return true;
    }
#elif defined(USE_POSIX_API)
    struct winsize w = {};
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0)
    {
        o_width = w.ws_col;
        o_height = w.ws_row;
        return true;
    }
#endif
    return false;
}

// One screen cell: the SGR (colour) sequence that was active when the glyph was written,
// plus the glyph itself. An empty glyph means the cell is blank.
struct Cell
{
    std::string sgr;
    std::string glyph;
};

// Emulate a frame's escape stream into a fixed-width cell grid - a small terminal model.
// The frame mixes newline-terminated text with cursor-positioned content (the card grid
// uses save/restore and relative moves) and long lines that the real terminal wraps at
// the right margin. Modelling all of that here (including wrapping at i_width) means the
// grid matches exactly what the terminal would show, so the renderer can then repaint it
// cleanly with absolute positioning instead of replaying fragile relative moves.
std::vector<std::vector<Cell>> EmulateFrame(const std::string& i_frame, int i_width)
{
    const Cell blank{};
    std::vector<std::vector<Cell>> grid;
    auto ensureRow = [&](int r)
    {
        while (static_cast<int>(grid.size()) <= r)
        {
            grid.emplace_back(i_width, blank);
        }
    };

    int row = 0;
    int col = 0;
    int savedRow = 0;
    int savedCol = 0;
    std::string sgr;
    const size_t n = i_frame.size();
    for (size_t i = 0; i < n;)
    {
        const unsigned char c = static_cast<unsigned char>(i_frame[i]);
        if (c == 0x1b && i + 1 < n && i_frame[i + 1] == '[')
        {
            size_t j = i + 2;
            while (j < n && !(i_frame[j] >= 0x40 && i_frame[j] <= 0x7e))
            {
                ++j;
            }
            const char fb = (j < n) ? i_frame[j] : '\0';
            long p[2] = {0, 0};
            bool has[2] = {false, false};
            int pi = 0;
            bool priv = false;
            for (size_t k = i + 2; k < j; ++k)
            {
                const char pc = i_frame[k];
                if (pc >= '0' && pc <= '9')
                {
                    p[pi] = p[pi] * 10 + (pc - '0');
                    has[pi] = true;
                }
                else if (pc == ';')
                {
                    if (pi < 1)
                    {
                        ++pi;
                    }
                }
                else
                {
                    priv = true; // private marker like '?' (e.g. "\033[?25l")
                }
            }
            if (fb == 'm')
            {
                const std::string seq = i_frame.substr(i, j - i + 1);
                if (seq == "\033[m" || seq == "\033[0m")
                {
                    sgr.clear();
                }
                else
                {
                    sgr = seq;
                }
            }
            else if (!priv)
            {
                const long n1 = has[0] ? p[0] : 1;
                switch (fb)
                {
                case 'A': row = (std::max)(0, row - static_cast<int>(n1)); break;
                case 'B': row += static_cast<int>(n1); break;
                case 'C': col = (std::min)(i_width - 1, col + static_cast<int>(n1)); break;
                case 'D': col = (std::max)(0, col - static_cast<int>(n1)); break;
                case 'E': row += static_cast<int>(n1); col = 0; break;
                case 'F': row = (std::max)(0, row - static_cast<int>(n1)); col = 0; break;
                case 'G': col = (std::min)(i_width - 1, (std::max)(0, static_cast<int>(n1) - 1)); break;
                case 'd': row = (std::max)(0, static_cast<int>(n1) - 1); break;
                case 'H':
                case 'f':
                {
                    const long n2 = has[1] ? p[1] : 1;
                    row = (std::max)(0, static_cast<int>(n1) - 1);
                    col = (std::min)(i_width - 1, (std::max)(0, static_cast<int>(n2) - 1));
                }
                break;
                case 's': savedRow = row; savedCol = col; break;
                case 'u': row = savedRow; col = savedCol; break;
                case 'K':
                {
                    ensureRow(row);
                    int start = col;
                    int end = i_width;
                    if (has[0] && p[0] == 1) { start = 0; end = col + 1; }
                    else if (has[0] && p[0] == 2) { start = 0; end = i_width; }
                    for (int cc = (std::max)(0, start); cc < end && cc < i_width; ++cc)
                    {
                        grid[row][cc] = blank;
                    }
                }
                break;
                default: break;
                }
            }
            i = (j < n) ? j + 1 : j;
        }
        else if (c == 0x1b)
        {
            i += (i + 1 < n) ? 2 : 1; // non-CSI escape; skip it
        }
        else if (c == '\n')
        {
            ++row;
            col = 0;
            ++i;
        }
        else if (c == '\r')
        {
            col = 0;
            ++i;
        }
        else if (c < 0x20)
        {
            ++i; // other control characters do not paint a cell
        }
        else
        {
            // Gather one (possibly multi-byte UTF-8) glyph.
            size_t g = i + 1;
            while (g < n && (static_cast<unsigned char>(i_frame[g]) & 0xC0) == 0x80)
            {
                ++g;
            }
            if (col < i_width)
            {
                ensureRow(row);
                grid[row][col] = Cell{sgr, i_frame.substr(i, g - i)};
            }
            ++col;
            if (col >= i_width) // wrap at the right margin, like a real terminal
            {
                col = 0;
                ++row;
            }
            i = g;
        }
    }
    return grid;
}

// Flatten one grid row to an output string spanning the full width, emitting an SGR
// sequence only when the colour changes. Writing the full width guarantees that any
// stale character left from a previous frame at any column is overwritten.
std::string RenderRow(const std::vector<Cell>& i_row, int i_width)
{
    std::string line;
    std::string active;
    bool activeSet = false;
    for (int cc = 0; cc < i_width; ++cc)
    {
        const Cell& cell = (cc < static_cast<int>(i_row.size())) ? i_row[cc] : Cell{};
        if (!activeSet || cell.sgr != active)
        {
            line += cell.sgr.empty() ? std::string("\033[0m") : cell.sgr;
            active = cell.sgr;
            activeSet = true;
        }
        line += cell.glyph.empty() ? std::string(" ") : cell.glyph;
    }
    line += "\033[0m";
    return line;
}
}

void Application::Render()
{
    if (!m_frameResult.renderStream)
    {
        return;
    }
    const std::string frame = m_frameResult.renderStream->str();
    if (m_previousFrameResult.renderStream && m_previousFrameResult.renderStream->str() == frame)
    {
        return; // identical frame - nothing changed, so emit nothing (no flicker)
    }

    ScopedVirtualConsoleMode();

    if (m_renderWidth <= 0)
    {
        int w = 0, h = 0;
        QueryTerminalSize(w, h);
        m_renderWidth = (w > 0) ? w : 120;
        m_renderHeight = (h > 0) ? h : 30;
    }
    const int width = m_renderWidth;

    // Emulate the frame into a virtual cell grid, then render that grid with absolute
    // positioning and a per-row diff against the previous frame. Only rows that actually
    // changed are rewritten (so there is no full-screen clear and therefore no flicker),
    // and every rewritten row spans the full width (so no stale character can survive).
    const std::vector<std::vector<Cell>> grid = EmulateFrame(frame, width);

    std::vector<std::string> rows;
    rows.reserve(grid.size());
    for (const std::vector<Cell>& gridRow : grid)
    {
        rows.push_back(RenderRow(gridRow, width));
    }
    // Clamp to the visible terminal height. Absolute positioning (\033[r;1H) is only
    // valid for on-screen rows: addressing rows past the bottom makes the terminal
    // scroll, which permanently desyncs this diff against m_previousRows and leaves
    // stale content behind when a later, shorter frame is drawn. Frames taller than the
    // screen simply have their overflow rows dropped instead of corrupting the diff.
    if (m_renderHeight > 0 && rows.size() > static_cast<size_t>(m_renderHeight))
    {
        rows.resize(m_renderHeight);
    }

    std::string output;
    output += "\033[?25l\033[?7l"; // hide cursor; disable auto-wrap so full-width rows never spill
    if (m_firstRender)
    {
        output += "\033[H\033[2J"; // one-time clear so no pre-existing content lingers
        m_firstRender = false;
    }
    for (size_t r = 0; r < rows.size(); ++r)
    {
        if (r >= m_previousRows.size() || m_previousRows[r] != rows[r])
        {
            output += utils::Format("\033[{};1H", r + 1);
            output += rows[r];
        }
    }
    // Clear rows the previous frame used but this one no longer does (content shrank).
    for (size_t r = rows.size(); r < m_previousRows.size(); ++r)
    {
        output += utils::Format("\033[{};1H\033[2K", r + 1);
    }
    output += "\033[?7h"; // restore auto-wrap

    std::cout << output << std::flush;
    m_previousRows = std::move(rows);
    m_previousFrameResult = std::move(m_frameResult);
}

void Application::SyncWithFrameThread()
{
    m_frameThread.Submit(GetElapsedSeconds());
    m_frameThread.Wait();
}
