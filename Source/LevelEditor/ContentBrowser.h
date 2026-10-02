#include <Engine/Core/Engine.h>
#include <Engine/Asset/AssetSubsystem.h>
#include <Engine/Asset/Texture/Texture2D.h>
#include <filesystem>
#include <vector>
#include <imgui.h>

namespace fs = std::filesystem;

class ContentBrowser {
public:
	ContentBrowser(const fs::path& startingPath = fs::current_path()) : _currentDirectory(startingPath) {}

	void DrawWindow() {
		auto assetSystem = CusEngine::Engine::Get()->GetSubsystem<CusEngine::AssetSubsystem>();

		ImGui::Begin("Content Browser");

		if (_currentDirectory.has_parent_path()) {
			if (ImGui::Button("<- Up")) {
				_currentDirectory = _currentDirectory.parent_path();
			}
			ImGui::SameLine();
		}

		ImGui::TextWrapped("%s", _currentDirectory.string().c_str());
		ImGui::Separator();

		float thumbnailSize = 90.0f;
		float padding = 2.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1) columnCount = 1;

		try {
			if (ImGui::BeginTable("ContentBrowserGrid", columnCount)) {
				for (auto& directoryEntry : fs::directory_iterator(_currentDirectory)) {
					ImGui::TableNextColumn();

					const auto& path = directoryEntry.path();
					std::string filenameString = path.filename().string();
					bool isDirectory = directoryEntry.is_directory();

					std::string label = isDirectory ? "[D] " + filenameString : "[F] " + filenameString;
					ImGui::PushID(filenameString.c_str());

					if (ImGui::Button(label.c_str(), ImVec2(thumbnailSize, thumbnailSize))) {
						if (isDirectory) {
							_currentDirectory /= path.filename();
						}
						else if (path.extension() == ".casset") {
							auto asset = assetSystem->Get<CusEngine::Asset>(path.string());
							auto assetSource = assetSystem->GetAssetSource(asset->GetAssetHandle());

							if (assetSource.type == CusEngine::Texture2D::StaticClassName().data()) {
								CusEngine::Texture2D* texture = static_cast<CusEngine::Texture2D*>(asset);
								_selectedTexture = texture;
								_editingDesc = *texture->_image->GetDesc();
							}

							Logger::Info(path.filename().string(), "{}", asset->GetAssetHandle().Str());
						}
					}

					if (ImGui::IsItemHovered()) {
						ImGui::BeginTooltip();
						ImGui::Text("%s", filenameString.c_str());
						ImGui::EndTooltip();
					}

					ImGui::PopID();
				}
				ImGui::EndTable();
			}
		}
		catch (const std::exception& e) {
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Error reading directory:");
			ImGui::TextWrapped("%s", e.what());
		}

		ImGui::End();

		ImGui::Begin("Properties");

		if (_selectedTexture != nullptr) {
			ImGui::Text("Selected Texture: %s", _selectedTexture->GetAssetHandle().Str().c_str());
			ImGui::Separator();

			DrawImageDescEditor(_editingDesc);

			ImGui::Spacing();

			if (ImGui::Button("Apply Changes", ImVec2(-1, 0))) {
				_selectedTexture->_image->SetDesc(_editingDesc);
				
				assetSystem->Reimport(_selectedTexture->GetAssetHandle());
			}
		}
		else {
			ImGui::TextDisabled("Select an asset to view properties.");
		}

		ImGui::End();
	}

	void DrawImageDescEditor(Runtime::RHI::ImageDesc& desc) {
		ImGui::Text("Image Descriptor");
		ImGui::Separator();

		ImGui::InputScalar("Width", ImGuiDataType_U32, &desc.width);
		ImGui::InputScalar("Height", ImGuiDataType_U32, &desc.height);

		const char* samplerNames[] = { 
			"PointClamp",
			"PointWrap",
			"LinearClamp",
			"LinearWrap",
			"LinearMirror",
			"NearestClamp",
			"NearestRepeat",
			"AnisoClamp",
			"AnisoMirror",
			"ShadowCompare",
		};
		int currentSampler = static_cast<int>(desc.sampler);
		if (ImGui::Combo("Sampler", &currentSampler, samplerNames, IM_ARRAYSIZE(samplerNames))) {
			desc.sampler = static_cast<Runtime::RHI::StaticSampler>(currentSampler);
		}

		const char* formatNames[] = { 
			"Undefined",
			"RG8",
			"RGB8",
			"RGBA8",
			"RGBA16",
			"RGBA",
			"BC3",
			"BC5",
			"BC7",
			"D32_SFLOAT",
			"D16_UNORM",
			"D24_UNORM_S8_UINT"
		};
		int currentFormat = static_cast<int>(desc.format);
		if (ImGui::Combo("Format", &currentFormat, formatNames, IM_ARRAYSIZE(formatNames))) {
			desc.format = static_cast<Runtime::RHI::Format>(currentFormat);
		}

		const char* tileModeNames[] = { 
			"Undefined",
			"Repeat",
			"Mirror",
			"ClampToEdge",
			"ClampToBorder",
			"Optimal"
		};
		int currentTileMode = static_cast<int>(desc.tileMode);
		if (ImGui::Combo("Tile Mode", &currentTileMode, tileModeNames, IM_ARRAYSIZE(tileModeNames))) {
			desc.tileMode = static_cast<Runtime::RHI::ImageTileMode>(currentTileMode);
		}
	}
private:
	fs::path _currentDirectory;

	CusEngine::Texture2D* _selectedTexture = nullptr;
	Runtime::RHI::ImageDesc _editingDesc;
};