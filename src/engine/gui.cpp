#include "gui.hpp"
#include "object.hpp"

#include <cstdint>
#include <ranges>

engine::GUI::GUI(const engine::Renderer &renderer) : renderer(renderer) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    auto &io = ImGui::GetIO();
    (void)io;
    ImGui_ImplGlfw_InitForOpenGL(renderer.getWindow(), true);
    ImGui_ImplOpenGL3_Init();
}

engine::GUI::~GUI() {
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
}

void engine::GUI::render(
    ImVec2 gameView, uint32_t gameViewTexture,
    const std::vector<std::reference_wrapper<Object>> &objects) const {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::ShowMetricsWindow();
    ImGui::ShowDemoWindow();

    // Render game to viewport
    ImGuiWindowFlags gvFlags = 0;
    gvFlags |= ImGuiWindowFlags_NoCollapse;

    ImGui::Begin("Game View", nullptr, gvFlags);
    ImGui::Image(
        reinterpret_cast<void *>(static_cast<intptr_t>(gameViewTexture)),
        gameView, ImVec2(0, 1), ImVec2(1, 0));
    ImGui::End();

    static unsigned int selectedComponent = 0;

    ImGui::Begin("Objects");
    for (auto [i, obj] : std::views::zip(std::views::iota(1), objects)) {
        ImGui::Bullet();
        const char *name = obj.get().name.c_str();
        if (ImGui::SmallButton(name)) {
            selectedComponent = i;
        }
    }
    ImGui::End();

    if (selectedComponent > 0) {
        Object &obj = objects[selectedComponent - 1].get();
        auto &transforms = obj.transforms;

        // clang-format off
        ImGui::Begin(obj.name.c_str());
        ImGui::Text("Transform");
        ImGui::Indent();
            ImGui::InputFloat2("Translate", transforms.translate);
            ImGui::InputFloat2("Scale", transforms.scale);
            ImGui::SliderFloat("Rotate", &transforms.rotate, -360.0f, 360.0f);
        ImGui::Unindent();
        ImGui::End();
        // clang-format on
    }
}
