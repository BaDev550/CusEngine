#pragma once

#include "Texture2D.h"
#include "RHI/RHI_Utils.h"
#include "RHI/RHI_Buffer.h"
#include "RHI/RHI_Image.h"
#include "RHI/RHI_RenderCommands.h"
#include "Core/Engine.h"

namespace CusEngine {
	Texture2D::Texture2D(RHI::RenderCommands* commands, const TextureDesc& desc, u8* data) : RenderObject(commands), _desc(desc) {
		RHI::ImageDesc imageDesc{};
		imageDesc.width = _desc.Width;
		imageDesc.height = _desc.Height;
		imageDesc.format = _desc.Format;
		imageDesc.usage = RHI::ImageUsage::Sampled | RHI::ImageUsage::TransferDst;
		//_image = Mem::Ref<RHI::Image>(RHI::CreateImage(commands, imageDesc));

		//commands->Submit([this, commands, data]() {
		//	size_t bufferSize = (_desc.Width * _desc.Height * 4); // TODO(0x): switch 4 witch a format to component count switch
		//
		//	RHI_BufferDesc bufferDesc{};
		//	bufferDesc.Size = bufferSize;
		//	bufferDesc.Usage = RHI_BufferUsage::TransferSrc;
		//	bufferDesc.MemoryUsage = RHI_MemoryUsage::Auto;
		//	bufferDesc.AllocationFlags = RHI_AllocationFlagBits::HostAccessSequentialWrite | RHI_AllocationFlagBits::CreateMapped;
		//	Memory::Ref<RHI_Buffer> stagingBuffer = Memory::Ref<RHI_Buffer>(RHI_CreateBuffer(commands, bufferDesc));
		//	
		//	stagingBuffer->Write(data);
		//
		//	commands->Track(stagingBuffer);
		//
		//	commands->TransitionImageLayout(_image.Get(), RHI_ImageLayout::TransferDst);
		//	commands->CopyBufferToImage(stagingBuffer.Get(), _image.Get(), RHI_ImageLayout::TransferDst, _desc.Width, _desc.Height);
		//	commands->TransitionImageLayout(_image.Get(), RHI_ImageLayout::ShaderReadOnly);
		//	Logger::Info("Texture2D", "Texture loaded into gpu!");
		//	});
	}

	Texture2D::~Texture2D() {
		_image = nullptr;
	}

	u32 Texture2D::GetBindlessID()
	{
		if (_bindlessID == u32_max)
			return GetRenderCommands()->RegisterBindlessImage(_image);
		return _bindlessID;
	}
}