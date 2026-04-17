#include "../ff/core/attribute.h"
#include "../ff/core/geometry.h"
#include "../ff/objects/mesh.h"
#include "../ff/scene/scene.h"
#include "../ff/camera/perspectiveCamera.h"
#include "../ff/cameraControl/gameCameraControl.h"
#include "../ff/render/renderer.h"
#include "../ff/material/meshBasicMaterial.h"
#include "../ff/log/debugLog.h"
#include "../ff/global/constant.h"


uint32_t WIDTH = 800;
uint32_t HEIGHT = 600;


static void onMouseMove(double xpos, double ypos) 
{

}

static void onMouseAction(ff::MouseAction action)
{
	if (action == ff::MouseAction::LeftDown)
	{
		std::cout << "LeftDown" << std::endl;
	}
}

static void onKeyboardAction(KeyBoardState action)
{

}

static void onResize(int width, int height)
{

}

int main()
{
	std::vector<float> positions =
	{
		-0.5f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f,
		0.5f, 0.0f, 0.0f,
	};

	std::vector<float> colors =
	{
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f
	};
	
	std::vector<uint32_t> indices =
	{
		0, 1, 2
	};

	//geometry
	auto geometry = ff::Geometry::create();
	geometry->setAttribute("position", ff::Attributef::create(positions, 3));
	geometry->setAttribute("color", ff::Attributef::create(colors, 3));
	geometry->setIndex(ff::Attributei::create(indices, 1));

	//material
	auto material = ff::MeshBasicMaterial::create();

	auto mesh = ff::Mesh::create(geometry, material);

	auto secne = ff::Scene::create();
	secne->addChild(mesh);

	auto camera = ff::PerspectiveCamera::create(0.1f, 100.0f, (float)WIDTH / (float)HEIGHT, 60.0f);
	
	camera->setPosition(0.0f, 0.0f, 1.0f);

	//渲染器
	ff::Renderer::Descriptor rDc;
	rDc.mWidth = WIDTH;
	rDc.mHeight = HEIGHT;
	ff::Renderer::Ptr renderer = ff::Renderer::create(rDc);

	renderer->setClearColor(0.94, 1.0, 0.94, 1.0);

	renderer->setMouseActionCallback(onMouseAction);
	renderer->setKeyboardActionCallBack(onKeyboardAction);
	renderer->setFrameSizeCallBack(onResize);
	renderer->setMouseMoveCallBack(onMouseMove);


	while (true)
	{
		if (!renderer->render(secne, camera))
		{
			break;
		}

		renderer->swap();
	}

}
