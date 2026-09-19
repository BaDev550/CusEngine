#pragma once

#include "Texture.h"
#include "RHI/RHI_Utils.h"
#include "RHI/RHI_Buffer.h"
#include "RHI/RHI_Image.h"
#include "Core/Engine.h"

namespace Graphics {
	Texture2D::Texture2D(const TextureDesc& desc, u8* data) : _desc(desc) {
		RHI_RenderCommands* commands = Engine::Get()->GetRenderer()->GetRenderCommands(); // Use DI

		RHI_ImageDesc imageDesc{};
		imageDesc.Width = _desc.Width;
		imageDesc.Height = _desc.Height;
		imageDesc.Format = _desc.Format;
		imageDesc.Usage = RHI_ImageUsage::Sampled | RHI_ImageUsage::TransferDst;
		_image = Memory::Ref<RHI_Image>(RHI_CreateImage(commands, imageDesc));

		commands->Submit([this, commands, data](RHI_CommandBufferHandle cmd) {
			size_t bufferSize = (_desc.Width * _desc.Height * 4); // TODO(0x): switch 4 witch a format to component count switch

			RHI_BufferDesc bufferDesc{};
			bufferDesc.Size = bufferSize;
			bufferDesc.Usage = RHI_BufferUsage::TransferSrc;
			bufferDesc.MemoryUsage = RHI_MemoryUsage::Auto;
			bufferDesc.AllocationFlags = RHI_AllocationFlagBits::HostAccessSequentialWrite | RHI_AllocationFlagBits::CreateMapped;
			Memory::Ref<RHI_Buffer> stagingBuffer = Memory::Ref<RHI_Buffer>(RHI_CreateBuffer(commands, bufferDesc));
			
			stagingBuffer->Write(data);

			commands->Track(stagingBuffer);

			commands->TransitionImageLayout(_image.Get(), RHI_ImageLayout::TransferDst);
			commands->CopyBufferToImage(stagingBuffer.Get(), _image.Get(), RHI_ImageLayout::TransferDst, _desc.Width, _desc.Height);
			commands->TransitionImageLayout(_image.Get(), RHI_ImageLayout::ShaderReadOnly);
			Logger::Info("Texture2D", "Texture loaded into gpu!");
			});
	}

	Texture2D::~Texture2D() {
		_image = nullptr;
	}

	u32 Texture2D::GetBindlessID()
	{
		RHI_RenderCommands* commands = Engine::Get()->GetRenderer()->GetRenderCommands(); // Use DI

		if (_bindlessID == u32_max)
			return commands->RegisterBindlessImage(_image);
		return _bindlessID;
	}
}