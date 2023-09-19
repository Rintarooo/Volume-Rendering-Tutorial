#include <cstdio>
#include "VolumeRenderer.h"
#include "Grid.h"

void populateGrid_sphere(Grid* grid) {
	const int* dims = grid->getDimensions();
	for(int i=0;i<dims[0]; ++i)
		for(int j=0;j<dims[1]; ++j)
			for(int k=0;k<dims[2]; ++k) {

				Vec3 voxelPosition;
				voxelPosition.x = (float)i / (dims[0]-1) * 2.f + -1.f;
				voxelPosition.y = (float)j / (dims[1]-1) * 2.f + -1.f;
				voxelPosition.z = (float)k / (dims[2]-1) * 2.f + -1.f;

				// Fill the grid with a solid sphere with a very dense inner sphere
				float dfield2 = voxelPosition.normSquared();
				float value = dfield2 < 1.f ? 1.f : 0.f;

				if(dfield2<.25f)
					value = 20.f;

				grid->setVoxel(i,j,k,value);	
			}
}

void populateGrid_cube(Grid* grid) {
	const int* dims = grid->getDimensions();
	for(int i=0;i<dims[0]; ++i)
		for(int j=0;j<dims[1]; ++j)
			for(int k=0;k<dims[2]; ++k) {

				Vec3 voxelPosition;
				voxelPosition.x = (float)i / (dims[0]-1) * 2.f + -1.f;
				voxelPosition.y = (float)j / (dims[1]-1) * 2.f + -1.f;
				voxelPosition.z = (float)k / (dims[2]-1) * 2.f + -1.f;

				// Fill the grid with a solid sphere with a very dense inner sphere
				float value = 0.3f;
				const float thres_pos = 0.5f;
				if(-thres_pos < voxelPosition.x && voxelPosition.x < thres_pos)
					if(-thres_pos < voxelPosition.y && voxelPosition.y < thres_pos)
						if(-thres_pos < voxelPosition.z && voxelPosition.z < thres_pos)
							value = 0.9f;

				grid->setVoxel(i,j,k,value);	
			}
}

int main(int argc, char* const argv[]) {
	printf("hello!!!!\n");

	const int grid_resolution = 128;//16;//  
	Grid grid(grid_resolution, grid_resolution, grid_resolution, Vec3(-1,-1,-1), Vec3(1,1,1));	

	const int render_img_resolution = 256;//32;//64;//16// , 512
	printf("render_img_resolution: %d x %d\n", render_img_resolution, render_img_resolution);
	VolumeRenderer renderer(render_img_resolution, render_img_resolution, &grid);

	// Let's fill the grid with something to render.
	populateGrid_sphere(&grid);
	// populateGrid_cube(&grid);

	// // Ray marching
	// renderer.render(Vec3(1,1,1), Vec3(0,0,0), 2, 128);
	Vec3 cameraPosition(1.45, 1.1, 3.1);//(1, 1, 1);//(0, 0, -2);// 
	Vec3 cameraFocus(0, 0, 0);
	float marchDistance = 4;// 2;
	int marchingStepCount = 128;// 15;
	renderer.render(cameraPosition, cameraFocus, marchDistance, marchingStepCount);

	// Output an image to look at.
	renderer.writeImage("output.png");

	return 0;
}
