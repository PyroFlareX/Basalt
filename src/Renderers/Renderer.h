#ifndef RENDERER_H
#define RENDERER_H

#include "../Camera.h"
#include "GeneralRenderer.h"
#include "ComputeRenderer.h"
#include "../../Engine/Engine.h"


class Renderer
{
    public:
        Renderer();

		void drawObject(vn::GameObject& entity);
		void doCompute();
		void render(Camera& cam);
		void finish();

		void clearQueue();

        ~Renderer();
    protected:

    private:
		GeneralRenderer m_generalRenderer;
		ComputeRenderer m_computeRenderer;


};

#endif // RENDERER_H
