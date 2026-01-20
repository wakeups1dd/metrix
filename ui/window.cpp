#include "window.h"
#include "../common/logger.h"
#include "../common/config.h"
#include <imgui.h>
#include <imgui_internal.h> // Required for direct DrawList access
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <mutex>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#include <d3d11.h>
#endif

// Forward declare ImGui Win32 message handler
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace openmeters::ui {

Window::Window() {
    m_config = common::ConfigManager::get();
}

Window::~Window() {
    shutdown();
}

bool Window::initialize(HINSTANCE hInstance, int nCmdShow) {
    m_hInstance = hInstance;
    
    LOG_INFO("Initializing window...");
    
    if (!createWindow(hInstance, nCmdShow)) {
        LOG_ERROR("Failed to create window");
        return false;
    }
    
    if (!initializeD3D11()) {
        LOG_ERROR("Failed to initialize DirectX 11");
        return false;
    }
    
    if (!initializeImGui()) {
        LOG_ERROR("Failed to initialize ImGui");
        return false;
    }
    
    LOG_INFO("Window initialized successfully");
    return true;
}

bool Window::createWindow(HINSTANCE hInstance, int nCmdShow) {
    const char* className = "OpenMetersWindow";
    
    // Register window class
    WNDCLASSEXA wc = {};
    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.style = CS_CLASSDC;
    wc.lpfnWndProc = windowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = className;
    
    if (!RegisterClassExA(&wc)) {
        return false;
    }
    
    // Create window
    int width = static_cast<int>(m_config.windowWidth);
    int height = static_cast<int>(m_config.windowHeight);
    int x = static_cast<int>(m_config.windowX);
    int y = static_cast<int>(m_config.windowY);
    
    m_hWnd = CreateWindowExA(
        WS_EX_TOPMOST | WS_EX_LAYERED,  // Always on top
        className,
        "OpenMeters",
        WS_POPUP | WS_VISIBLE,
        x, y, width, height,
        nullptr, nullptr, hInstance, this
    );
    
    if (!m_hWnd) {
        return false;
    }
    
    // Set window transparency
    SetLayeredWindowAttributes(m_hWnd, 0, 255, LWA_ALPHA);
    
    ShowWindow(m_hWnd, nCmdShow);
    UpdateWindow(m_hWnd);
    
    return true;
}

bool Window::initializeD3D11() {
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = m_hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    
    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_0,
    };
    
    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        createDeviceFlags,
        featureLevelArray,
        2,
        D3D11_SDK_VERSION,
        &sd,
        &m_swapChain,
        &m_device,
        &featureLevel,
        &m_context
    );
    
    if (FAILED(hr)) {
        return false;
    }
    
    // Create render target view
    ID3D11Texture2D* pBackBuffer = nullptr;
    m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
    if (pBackBuffer) {
        m_device->CreateRenderTargetView(pBackBuffer, nullptr, &m_renderTargetView);
        pBackBuffer->Release();
    }
    
    return true;
}

bool Window::initializeImGui() {
    // Setup ImGui context
    IMGUI_CHECKVERSION();
    m_imguiContext = ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.IniFilename = nullptr; // Disable imgui.ini
    
    // Setup style
    if (m_config.darkMode) {
        ImGui::StyleColorsDark();
    } else {
        ImGui::StyleColorsLight();
    }
    
    setupStyle();
    
    // Setup platform/renderer backends
    ImGui_ImplWin32_Init(m_hWnd);
    ImGui_ImplDX11_Init(m_device, m_context);
    
    return true;
}

void Window::run() {
    MSG msg = {};
    
    while (!m_shouldClose) {
        // Process Windows messages
        while (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
            if (msg.message == WM_QUIT) {
                m_shouldClose = true;
            }
        }
        
        if (m_shouldClose) {
            break;
        }
        
        // Render frame
        renderFrame();
        
        // Sleep to limit CPU usage
        Sleep(16); // ~60 FPS
    }
}

void Window::renderFrame() {
    // Start ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    
    // Render main window
    renderMeters();
    
    // Render settings if open
    if (m_showSettings) {
        renderSettings();
    }
    
    // Render ImGui
    ImGui::Render();
    
    // Clear render target
    const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    m_context->OMSetRenderTargets(1, &m_renderTargetView, nullptr);
    m_context->ClearRenderTargetView(m_renderTargetView, clearColor);
    
    // Draw ImGui
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    
    // Present
    m_swapChain->Present(1, 0); // VSync
}

void Window::renderMeters() {
    // Get current meter values (thread-safe)
    common::MeterSnapshot snapshot;
    {
        std::lock_guard<std::mutex> lock(m_meterMutex);
        snapshot = m_currentSnapshot;
    }
    
    // Create main window (no title bar, no background)
    ImGuiWindowFlags flags = 
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoDecoration;
    
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    
    ImGui::Begin("Meters", nullptr, flags);
    
    // Window dragging logic
    if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
        POINT p;
        if (GetCursorPos(&p)) {
            // Get current window pos
            RECT rect;
            GetWindowRect(m_hWnd, &rect);
            
            // Move window
            // Note: This is a simplified drag implementation. 
            // For production, we'd calculate delta from previous frame.
            // Since this runs every frame, we just need to handle the delta.
            // However, ImGui doesn't give us raw delta easily without storing state.
            // Better approach: Let Windows handle it via HTCAPTION
            
            ReleaseCapture();
            SendMessageA(m_hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
        }
    }
    
    // Draw peak meters
    if (m_config.showPeakMeter) {
        ImGui::Text("Peak");
        drawMeter("##PeakL", snapshot.peak.left, ImVec2(-1, 20));
        drawMeter("##PeakR", snapshot.peak.right, ImVec2(-1, 20));
    }
    
    ImGui::Spacing();
    
    // Draw RMS meters
    if (m_config.showRmsMeter) {
        ImGui::Text("RMS");
        drawMeter("##RmsL", snapshot.rms.left, ImVec2(-1, 20));
        drawMeter("##RmsR", snapshot.rms.right, ImVec2(-1, 20));
    }
    
    // Settings button
    if (ImGui::Button("Settings")) {
        m_showSettings = !m_showSettings;
    }
    
    ImGui::End();
}

void Window::setupStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 8.0f;
    style.FrameRounding = 4.0f;
    style.PopupRounding = 4.0f;
    style.ScrollbarRounding = 4.0f;
    style.GrabRounding = 4.0f;
    
    // Custom colors (Dark/Professional)
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.12f, 0.12f, 0.12f, 0.85f); // Semi-transparent dark background
    colors[ImGuiCol_Border] = ImVec4(0.30f, 0.30f, 0.30f, 0.50f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.54f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.30f, 0.30f, 0.30f, 0.40f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.40f, 0.40f, 0.40f, 0.67f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 0.90f, 0.00f, 1.00f); // Green accents
    colors[ImGuiCol_SliderGrab] = ImVec4(0.00f, 0.90f, 0.00f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.20f, 1.00f, 0.20f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.25f, 0.25f, 0.25f, 0.40f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
}

void Window::drawMeter(const char* label, float value, const ImVec2& size) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return;
    
    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    
    ImVec2 pos = window->DC.CursorPos;
    ImVec2 actualSize = ImGui::CalcItemSize(size, ImGui::GetWindowWidth(), style.FramePadding.y * 2.0f);
    const ImRect bb(pos, ImVec2(pos.x + actualSize.x, pos.y + actualSize.y));
    
    ImGui::ItemSize(actualSize, style.FramePadding.y);
    if (!ImGui::ItemAdd(bb, id)) return;
    
    // Background
    window->DrawList->AddRectFilled(bb.Min, bb.Max, ImGui::GetColorU32(ImGuiCol_FrameBg), style.FrameRounding);
    
    // Segments
    // 20 segments total
    const int segments = 20;
    const float segmentSpacing = 2.0f;
    const float segmentWidth = (actualSize.x - (style.FramePadding.x * 2) - (segmentSpacing * (segments - 1))) / segments;
    const float segmentHeight = actualSize.y - (style.FramePadding.y * 2);
    
    // Clamp value 0-1
    float v = std::clamp(value, 0.0f, 1.0f);
    
    for (int i = 0; i < segments; i++) {
        float segmentThreshold = (float)i / (float)segments;
        
        if (v > segmentThreshold) {
            float x = bb.Min.x + style.FramePadding.x + (i * (segmentWidth + segmentSpacing));
            float y = bb.Min.y + style.FramePadding.y;
            
            ImU32 color;
            if (segmentThreshold >= 0.9f) { // Over -6dB -> Red
                color = IM_COL32(255, 50, 50, 255);
            } else if (segmentThreshold >= 0.7f) { // Over -18dB -> Yellow
                color = IM_COL32(255, 200, 50, 255);
            } else { // Normal -> Green
                color = IM_COL32(50, 255, 50, 255);
            }
            
            window->DrawList->AddRectFilled(
                ImVec2(x, y), 
                ImVec2(x + segmentWidth, y + segmentHeight), 
                color,
                2.0f // Rounding
            );
        }
    }
}

void Window::renderSettings() {
    ImGui::Begin("Settings", &m_showSettings);
    
    ImGui::Checkbox("Always On Top", &m_config.alwaysOnTop);
    ImGui::Checkbox("Show Peak Meter", &m_config.showPeakMeter);
    ImGui::Checkbox("Show RMS Meter", &m_config.showRmsMeter);
    ImGui::Checkbox("Dark Mode", &m_config.darkMode);
    
    ImGui::SliderFloat("UI Scale", &m_config.uiScale, 0.5f, 2.0f);
    ImGui::SliderFloat("Meter Update Rate", &m_config.meterUpdateRate, 30.0f, 120.0f);
    
    if (ImGui::Button("Save")) {
        common::ConfigManager::get() = m_config;
        common::ConfigManager::save();
        LOG_INFO("Settings saved");
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Close")) {
        m_showSettings = false;
    }
    
    ImGui::End();
}

void Window::shutdown() {
    if (m_imguiContext) {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext(m_imguiContext);
        m_imguiContext = nullptr;
    }
    
    if (m_renderTargetView) {
        m_renderTargetView->Release();
        m_renderTargetView = nullptr;
    }
    
    if (m_swapChain) {
        m_swapChain->Release();
        m_swapChain = nullptr;
    }
    
    if (m_context) {
        m_context->Release();
        m_context = nullptr;
    }
    
    if (m_device) {
        m_device->Release();
        m_device = nullptr;
    }
    
    if (m_hWnd) {
        DestroyWindow(m_hWnd);
        m_hWnd = nullptr;
    }
    
    LOG_INFO("Window shutdown complete");
}

void Window::updateMeters(const common::MeterSnapshot& snapshot) {
    std::lock_guard<std::mutex> lock(m_meterMutex);
    m_currentSnapshot = snapshot;
}

bool Window::shouldClose() const {
    return m_shouldClose;
}

LRESULT CALLBACK Window::windowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) {
        return true;
    }
    
    Window* window = nullptr;
    if (msg == WM_NCCREATE) {
        CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
        window = static_cast<Window*>(cs->lpCreateParams);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
    } else {
        window = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    }
    
    switch (msg) {
        case WM_SIZE:
            if (window && window->m_swapChain && wParam != SIZE_MINIMIZED) {
                if (window->m_renderTargetView) {
                    window->m_renderTargetView->Release();
                    window->m_renderTargetView = nullptr;
                }
                window->m_swapChain->ResizeBuffers(0, LOWORD(lParam), HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
                ID3D11Texture2D* pBackBuffer = nullptr;
                window->m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
                if (pBackBuffer) {
                    window->m_device->CreateRenderTargetView(pBackBuffer, nullptr, &window->m_renderTargetView);
                    pBackBuffer->Release();
                }
            }
            return 0;
            
        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU) {
                return 0; // Disable ALT application menu
            }
            break;
            
        case WM_DESTROY:
            if (window) {
                window->m_shouldClose = true;
            }
            PostQuitMessage(0);
            return 0;
    }
    
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

} // namespace openmeters::ui

