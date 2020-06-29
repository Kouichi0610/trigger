#include "VertexBuffer.h"

namespace dx {
	D3D12_VERTEX_BUFFER_VIEW VertexBuffer::GetView() const {
		return view;
	}
#if false
	int VertexBuffer::VertexCount() const {
		return vertexCount;
	}
	ComPtr<ID3D12Resource> VertexBuffer::GetVertexBuffer() const {
		return vertexBuffer;
	}
#endif
}
