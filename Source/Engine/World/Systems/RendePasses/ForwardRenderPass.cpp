#include "ForwardRenderPass.h"

namespace Tourqe::Engine {
	ForwardRenderPass::ForwardRenderPass() {
		Depends<RenderPass>();
	}
}