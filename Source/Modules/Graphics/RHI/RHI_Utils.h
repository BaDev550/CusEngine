#pragma once

#include "RHI.h"

extern "C" {
	namespace Graphics {
		namespace Utils {
			[[nodiscard]] __forceinline const char* RHI_BackendToString(RHI_Backend backend) noexcept {
				switch (backend)
				{
				case RHI_Backend::None: return "None";
				case RHI_Backend::Vulkan: return "Vulkan";
				case RHI_Backend::DirectX12: return "DirectX12";
				case RHI_Backend::OpenGL: return "OpenGL";
				default: return "Unknown api";
				}
			}

			[[nodiscard]] __forceinline const char* RHI_FormatToString(RHI_Format format) noexcept {
				switch (format)
				{
				case RHI_Format::Undefined: return "Undefined";
				case RHI_Format::RG8: return "RG8";
				case RHI_Format::RGB8: return "RGB8";
				case RHI_Format::RGBA8: return "RGBA8";
				case RHI_Format::RGBA16: return "RGBA16";
				case RHI_Format::RGBA: return "RGBA";
				case RHI_Format::D32_SFLOAT: return "D32 SFloat";
				case RHI_Format::D24_UNORM_S8_UINT: return "D24 Unorm S8 UInt";
				case RHI_Format::D16_UNORM: return "D16 Unorm";
				default: return "Unknown format";
				}
			}

			[[nodiscard]] __forceinline const char* RHI_ImageTileModeToString(RHI_ImageTileMode tileMode) noexcept {
				switch (tileMode)
				{
				case RHI_ImageTileMode::Undefined: return "Undefined";
				case RHI_ImageTileMode::Repeat: return "Repeat";
				case RHI_ImageTileMode::Mirror: return "Mirror";
				case RHI_ImageTileMode::ClampToEdge: return "ClampToEdge";
				case RHI_ImageTileMode::ClampToBorder: return "ClampToBorder";
				case RHI_ImageTileMode::Optimal: return "Optimal";
				default: return "Unknown tile mode";
				}
			}

			[[nodiscard]] __forceinline const char* RHI_ImageUsageToString(RHI_ImageUsage usage) noexcept {
				switch (usage)
				{
				case RHI_ImageUsage::None: return "None";
				case RHI_ImageUsage::Sampled: return "Sampled";
				case RHI_ImageUsage::Storage: return "Storage";
				case RHI_ImageUsage::ColorAttachment: return "ColorAttachment";
				case RHI_ImageUsage::DepthStencilAttachment: return "DepthStencilAttachment";
				case RHI_ImageUsage::TransferSrc: return "TransferSrc";
				case RHI_ImageUsage::TransferDst: return "TransferDst";
				default: return "Unknown usage";
				}
			}

			[[nodiscard]] __forceinline bool RHI_IsFormatDepthStencil(RHI_Format format) noexcept {
				switch (format)
				{
				case RHI_Format::D24_UNORM_S8_UINT: return true;
				default: return false;
				}
			}

			[[nodiscard]] __forceinline bool RHI_IsFormatDepth(RHI_Format format) noexcept {
				switch (format)
				{
				case RHI_Format::D16_UNORM: return true;
				case RHI_Format::D24_UNORM_S8_UINT: return true;
				case RHI_Format::D32_SFLOAT: return true;
				default: return false;
				}
			}

			[[nodiscard]] __forceinline u32 RHI_GetFormatSize(RHI_Format format) noexcept {
				switch (format)
				{
				case RHI_Format::Undefined: return 0;
				case RHI_Format::RG8: return 2;
				case RHI_Format::RGB8: return 3;
				case RHI_Format::RGBA8: return 4;
				case RHI_Format::RGBA16: return 8;
				case RHI_Format::RGBA: return 16;
				default: return 0;
				}
			}
		}
	}
}