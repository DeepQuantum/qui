#include <algorithm>
#include <array>
#include <cstdio>
#include <iterator>
#include <span>
#include <vector>

#include <qui.h>
#include <qui/code_window_ctre.hpp>

namespace {

constexpr float TitleBarHeight = 34.0F;
constexpr float MenuBarHeight = 27.0F;

void glfw_error_callback(int error, const char *description) {
    std::fprintf(stderr, "GLFW error %d: %s\n", error, description);
}

constexpr const char *SampleDcpl =
    "u16 #43DF4E5E85BFD47C(unknown arg_0, u16 arg_1) {\n"
    "    unknown var_0 = npc-get-awareness(arg_0, player);\n"
    "    unknown var_1 = npc-get-acquisition(arg_0, player);\n"
    "    u16 var_3;\n"
    "    string var_4;\n"
    "    if(var_0 == arg_1) {\n"
    "        u16 var_5;\n"
    "        if(arg_1 == 2) {\n"
    "            var_5 = 1;\n"
    "            var_4 = \"Has certain\"; // friendly awareness\n"
    "        } else {\n"
    "            unknown var_7 = #A548628CB635DC72(arg_0, arg_1);\n"
    "            if(var_7) {\n"
    "                var_5 = 0;\n"
    "                var_4 = \"Friend's awareness\";\n"
    "            } else if(arg_1 == 3 && !(var_1 > 0.00)) {\n"
    "                var_5 = 1;\n"
    "                var_4 = \"Missing, no rec\";\n"
    "            } else {\n"
    "                var_5 = 0;\n"
    "                var_4 = \"Not supported\";\n"
    "            }\n"
    "        }\n"
    "        var_3 = var_5;\n"
    "    } else {\n"
    "        var_3 = 1;\n"
    "        var_4 = \"Has none\";\n"
    "    }\n"
    "    return var_3;\n"
    "}\n";

std::span<const qui::code::rule> sample_dcpl_rules() {
    using qui::code::ctre_rule;
    using qui::code::to_u32;
    using qui::color::rgba;
    static const std::vector<qui::code::rule> rules = {
        ctre_rule<R"(//.*)">(to_u32(rgba(0x6A, 0x73, 0x80))),
        ctre_rule<R"RE(\b(if|else|while|for|return|foreach|match|state|block|event|track|statescript|options|declarations|using|as|far|near|not|and|or|in|lambda|start|end|update|null|struct|enum)\b)RE">(to_u32(rgba(0xC6, 0x78, 0xDD))),
        ctre_rule<R"(\b((var|arg)_\d+)\b)">(to_u32(rgba(0xE0, 0x6C, 0x75))),
        ctre_rule<R"RE("(\\.|[^"\\])*")RE">(to_u32(rgba(0x98, 0xC3, 0x79))),
        ctre_rule<R"RE((?:--|\b)[A-Za-z0-9_/@>#\-]+\??(?=\s*\())RE">(to_u32(rgba(0x61, 0xAF, 0xEF))),
        ctre_rule<R"RE(#[A-Z0-9]{16})RE">(to_u32(rgba(0x56, 0xB6, 0xC2))),
        ctre_rule<R"(\b(\d+)\b)">(to_u32(rgba(0xD1, 0x9A, 0x66))),
        ctre_rule<R"RE(\b(u0|u8|i8|u16|i16|u32|i32|u64|i64|f32|f64|bool|string|timer|vector|actor|symbol)\b\??)RE">(to_u32(rgba(0xE5, 0xC0, 0x7B))),
    };
    return rules;
}

void draw_menu_bar(float top_offset) {
    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + top_offset));
    ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, MenuBarHeight));

    constexpr ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_MenuBar;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0F, 0.0F));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0F);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0F);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, qui::color::retina_dark::MenuBarBackground);
    ImGui::PushStyleColor(ImGuiCol_MenuBarBg, qui::color::retina_dark::MenuBarBackground);

    ImGui::Begin("##qui_menu_bar", nullptr, flags);
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            ImGui::MenuItem("Open...", "Ctrl+O");
            ImGui::MenuItem("Save", "Ctrl+S", false, false);
            ImGui::Separator();
            ImGui::MenuItem("Close");
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Hex View", nullptr, true);
            ImGui::MenuItem("Data Inspector", nullptr, true);
            ImGui::MenuItem("Pattern Data", nullptr, true);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help")) {
            ImGui::MenuItem("About qui_base");
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
    ImGui::End();

    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(3);
}

void draw_hex_table() {
    static constexpr std::array<unsigned char, 128> bytes = {
        0x4D, 0x5A, 0x90, 0x00, 0x03, 0x00, 0x00, 0x00,
        0x04, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00,
        0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00,
        0x0E, 0x1F, 0xBA, 0x0E, 0x00, 0xB4, 0x09, 0xCD,
        0x21, 0xB8, 0x01, 0x4C, 0xCD, 0x21, 0x54, 0x68,
        0x69, 0x73, 0x20, 0x70, 0x72, 0x6F, 0x67, 0x72,
        0x61, 0x6D, 0x20, 0x63, 0x61, 0x6E, 0x6E, 0x6F,
        0x74, 0x20, 0x62, 0x65, 0x20, 0x72, 0x75, 0x6E,
        0x20, 0x69, 0x6E, 0x20, 0x44, 0x4F, 0x53, 0x20,
        0x6D, 0x6F, 0x64, 0x65, 0x2E, 0x0D, 0x0D, 0x0A,
        0x24, 0x00, 0x00, 0x00, 0x50, 0x45, 0x00, 0x00,
        0x64, 0x86, 0x06, 0x00, 0x33, 0x22, 0x11, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x22, 0x20,
    };

    const char *headers[] = {
        "Offset", "00", "01", "02", "03", "04", "05", "06", "07",
        "08", "09", "0A", "0B", "0C", "0D", "0E", "0F", "ASCII"
    };

    if (!qui::begin_table("hex_table", static_cast<int>(std::size(headers)), ImVec2(0.0F, 280.0F))) {
        return;
    }

    qui::table_headers(headers, static_cast<int>(std::size(headers)));

    for (int row = 0; row < 8; ++row) {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("0000%04X", row * 16);

        char ascii[17] = {};
        for (int column = 0; column < 16; ++column) {
            const unsigned char value = bytes[static_cast<std::size_t>(row * 16 + column)];
            ascii[column] = value >= 0x20 && value <= 0x7E ? static_cast<char>(value) : '.';

            ImGui::TableNextColumn();
            if (value == 0x4D || value == 0x5A || value == 0x50 || value == 0x45) {
                ImGui::PushStyleColor(ImGuiCol_Text, qui::color::retina_dark::Highlight);
                ImGui::Text("%02X", value);
                ImGui::PopStyleColor();
            } else {
                ImGui::Text("%02X", value);
            }
        }

        ImGui::TableNextColumn();
        ImGui::TextUnformatted(ascii);
    }

    qui::end_table();
}

void draw_inspector_table() {
    const char *headers[] = {"Type", "Value", "Endian"};
    const char *rows[][3] = {
        {"u8", "0x4D", "n/a"},
        {"u16", "0x5A4D", "little"},
        {"u32", "0x00905A4D", "little"},
        {"float", "1.329e-38", "little"},
        {"ASCII", "MZ", "n/a"},
    };

    if (!qui::begin_table("inspector_table", 3, ImVec2(0.0F, 165.0F))) {
        return;
    }

    qui::table_headers(headers, 3);

    for (const auto &row : rows) {
        ImGui::TableNextRow();
        for (const char *cell : row) {
            ImGui::TableNextColumn();
            ImGui::TextUnformatted(cell);
        }
    }

    qui::end_table();
}

void draw_component_demo(float top_offset) {
    static char search[128] = "4D 5A";
    static char goto_address[64] = "00000080";
    static char pattern_name[128] = "portable_executable.hexpat";
    static qui::message_box alert_box;
    static qui::message_box dialog_box;
    static const char *last_dialog_action = "(none yet)";

    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + top_offset));
    ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, viewport->Size.y - top_offset));

    constexpr ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::Begin("qui_base ImHex-style component demo", nullptr, window_flags);

    ImGui::BeginChild("sidebar", ImVec2(285.0F, 0.0F), true);
    qui::section_label("Provider");
    ImGui::Spacing();
    qui::text_label("Current source");
    ImGui::TextUnformatted("sample_portable_executable.bin");
    ImGui::Spacing();
    qui::status_label("READ ONLY", qui::color::retina_dark::AccentYellow);
    qui::status_label("ANALYZED", qui::color::retina_dark::AccentGreen);
    ImGui::Separator();
    qui::section_label("Navigation");
    ImGui::Spacing();
    qui::text_input("Search bytes", search, sizeof(search), "AA BB CC");
    qui::text_input("Goto address", goto_address, sizeof(goto_address), "00000000");
    ImGui::Separator();
    qui::section_label("Pattern");
    ImGui::Spacing();
    qui::text_input("Pattern file", pattern_name, sizeof(pattern_name), "name.hexpat");
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("workspace", ImVec2(0.0F, 0.0F), true);
    if (qui::begin_tabs("workspace_tabs")) {
        if (qui::begin_tab("Hex View")) {
            qui::section_label("Hex View");
            qui::text_label("A compact table using the ImHex Retina Dark table, tab, and selection colors.");
            ImGui::Spacing();
            draw_hex_table();
            qui::end_tab();
        }

        if (qui::begin_tab("Data Inspector")) {
            qui::section_label("Data Inspector");
            qui::text_label("Reusable labels, inputs, and tables for value-oriented tools.");
            ImGui::Spacing();
            draw_inspector_table();
            qui::end_tab();
        }

        if (qui::begin_tab("Pattern Data")) {
            qui::section_label("Pattern Data");
            qui::text_label("Nested tables keep binary structures scannable.");
            ImGui::Spacing();

            const char *headers[] = {"Field", "Offset", "Size", "Value"};
            if (qui::begin_table("pattern_table", 4, ImVec2(0.0F, 210.0F))) {
                qui::table_headers(headers, 4);
                const char *rows[][4] = {
                    {"dos_header.e_magic", "0x0000", "2", "MZ"},
                    {"dos_header.e_lfanew", "0x003C", "4", "0x00000080"},
                    {"nt_headers.Signature", "0x0080", "4", "PE"},
                    {"file_header.Machine", "0x0084", "2", "AMD64"},
                    {"file_header.Sections", "0x0086", "2", "6"},
                };

                for (const auto &row : rows) {
                    ImGui::TableNextRow();
                    for (const char *cell : row) {
                        ImGui::TableNextColumn();
                        ImGui::TextUnformatted(cell);
                    }
                }
                qui::end_table();
            }
            qui::end_tab();
        }

        if (qui::begin_tab("Code View")) {
            qui::section_label("Code View");
            qui::text_label("Read-only, syntax-highlighted dcpl viewer using CTRE and the dcpl-lint patterns.");
            ImGui::Spacing();
            qui::code::code_window("##sample_code", SampleDcpl, sample_dcpl_rules());
            qui::end_tab();
        }

        if (qui::begin_tab("Dialogs")) {
            qui::section_label("Message Boxes");
            qui::text_label("Themed modal dialogs built from qui::message_box.");
            ImGui::Spacing();

            if (ImGui::Button("Show alert")) {
                alert_box.popup_id = "##demo_alert";
                qui::open_alert(alert_box, "Heads up", "This is a simple alert with a single OK button.");
            }
            ImGui::SameLine();
            if (ImGui::Button("Show save dialog")) {
                dialog_box.popup_id = "##demo_dialog";
                dialog_box.title = "Unsaved changes";
                dialog_box.message = "You have unsaved changes. Do you want to save them before exiting?";
                dialog_box.buttons = {
                    qui::message_box_button{"Exit without saving", qui::color::retina_dark::AccentRed, true, true},
                    qui::message_box_button{"Go back"},
                    qui::message_box_button{"Save & Exit"},
                };
                dialog_box.open();
            }

            ImGui::Spacing();
            qui::text_label("Last dialog action:");
            ImGui::SameLine();
            ImGui::TextUnformatted(last_dialog_action);

            qui::end_tab();
        }

        qui::end_tabs();
    }
    ImGui::EndChild();

    ImGui::End();

    qui::draw_message_box(alert_box);
    switch (qui::draw_message_box(dialog_box)) {
        case 0: last_dialog_action = "Exit without saving"; break;
        case 1: last_dialog_action = "Go back"; break;
        case 2: last_dialog_action = "Save & Exit"; break;
        case qui::message_box_dismissed: last_dialog_action = "Dismissed (Esc)"; break;
        default: break;
    }
}

} // namespace

int main() {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
        return 1;
    }

    const char *glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    GLFWwindow *window = glfwCreateWindow(1280, 720, "qui_base demo", nullptr, nullptr);
    if (window == nullptr) {
        glfwTerminate();
        return 1;
    }
    glfwSetWindowSizeLimits(window, 720, 450, GLFW_DONT_CARE, GLFW_DONT_CARE);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    float x_scale = 1.0F;
    float y_scale = 1.0F;
    glfwGetWindowContentScale(window, &x_scale, &y_scale);
    const float dpi_scale = std::max(x_scale, y_scale);

    qui::apply_retina_dark_theme();
    qui::apply_dpi_scale(dpi_scale);
    qui::load_default_fonts(dpi_scale);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        qui::draw_glfw_window_bar(window, "qui_base - sample_portable_executable.bin", TitleBarHeight);
        draw_menu_bar(TitleBarHeight);
        draw_component_demo(TitleBarHeight + MenuBarHeight);

        ImGui::Render();

        int display_w = 0;
        int display_h = 0;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.06F, 0.06F, 0.06F, 1.00F);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
