#include "VolumeRenderer.h"
#include <cstdio>
#include <Magick++.h>

// #define STB_IMAGE_STATIC
// #define STB_IMAGE_IMPLEMENTATION
// #include "stb_image.h"
// // #ifndef STB_IMAGE_WRITE_IMPLEMENTATION
// // #define STBI_MSC_SECURE_CRT
// // http://blawat2015.no-ip.com/~mieki256/diary/202207.html
// #define STB_IMAGE_WRITE_STATIC
// #define STB_IMAGE_WRITE_IMPLEMENTATION
// #include "stb_image_write.h"
// // #endif


VolumeRenderer::VolumeRenderer(int _width, int _height, Grid* dataSource)
	: width(_width), height(_height), volumeData(dataSource)
{
	// 4 is rgba channel number
	int N = width * height * 4;

	// Allocate pixels for our output image (RGBA)
	image = new float[N];

	// http://chanhaeng.blogspot.com/2018/12/how-to-use-stbimagewrite.html
	 /*** NOTICE!! You have to use uint8_t array to pass in stb function  ***/
	// Because the size of color is normally 255, 8bit.
	// If you don't use this one, you will get a weird imge.
	// uint8_t* image = new uint8_t[N];

	// Clear the image to black
	for(int i=0;i<N;++i) {
		image[i] = 0.f;
		// image[i]=0;
	}
}

VolumeRenderer::~VolumeRenderer() {
	// Deconstructor should clean up image.
	delete[] image;
}

void VolumeRenderer::render(const Vec3& cameraPosition, const Vec3& cameraFocus, float marchDistance, int marchingStepCount) {
	// Compute a few things for the camera set up.
	// Given the camera's position and a point to focus on,
	// we can construct the view space.
	
	Vec3 lookDirection = (cameraFocus - cameraPosition).normalized();
	Vec3 camRight = lookDirection ^ Vec3(0,1,0); // Assume there is no camera "roll".　Vec3 operator^ cross calculation
	Vec3 camUp = camRight ^ lookDirection;

	// compute distance to the camera plane from the field of view, and aspect ratio
	const float fov = 60.f;//90.f;//35.f;//120.f;//80.f;//150.f;//45.f;//
	// degree2radian = fov*3.141592659f/180.f
	// カメラからスクリーンまでの距離
	float cameraPlaneDistance = 1.f / (2.f * tanf(fov*3.141592659f/180.f*.5f));
	const float aspect = (float)width / height;

	// Compute the step size for the ray marching.
	const float ds = marchDistance / marchingStepCount;
	
	// For indicating progress during render.
	int pi=0;

	// For each pixel...
	#pragma omp parallel for
	for(int px=0;px<width;++px) {
		for(int py=0;py<height;++py) {

				// Pixel index into the image array.
				int pndx = 4*(py*width+px);

				// Compute the ray direction for this pixel. Same as in standard ray tracing.
				// `px`は現在考慮しているピクセルの水平方向の位置（0から`width-1`まで）、`width`は画像の幅（ピクセル数）を表しています。
				// まず`px+.5f`でピクセルの中心を基準に位置を取得し、それを`(float)(width-1)`で割ることで0から1の範囲に正規化しています。
				// その後、`-.5f`を加えることで、値の範囲を-0.5から0.5に変更しています。
				float u = -.5f + (px+.5f) / (float)(width-1);
				u *= aspect; // Correction for square pixels on non-square image aspect ratio.
				float v =  .5f - (py+.5f) / (float)(height-1);

				// カメラの視線の方向（`lookDirection`）とカメラの投影面までの距離（`cameraPlaneDistance`）を掛け合わせて、カメラの投影面上の中心点を求めています。（// カメラからスクリーンの中心へのベクトル）
				Vec3 cam2screen_center = lookDirection * cameraPlaneDistance;
				// 次に、`camRight*u`と`camUp*v`では、カメラの右方向ベクトル（`camRight`）と正規化された水平位置（`u`）、カメラの上方向ベクトル（`camUp`）と正規化された垂直位置（`v`）をそれぞれ掛け合わせています。
				// これにより、ピクセルの位置に対応するカメラの投影面上の点を求めます。
				// 最後に、これらのベクトルを全て足し合わせることで、カメラの位置から投影面上の特定の点へ向かう光線の方向（`rayDirection`）を計算します。
				Vec3 rayDirection = cam2screen_center + camRight*u + camUp*v;
				rayDirection.normalize();

				// Initialize transmissivity to 1. We haven't "seen" anything yet.
				// Remember this is 1-alpha.
				float T = 1.f;

				// Scattering coefficient for this medium. Assumed to be homogenous.
				// More on this in later tutorials. Play with this and see what happens...
				const float kappa = 1.f;

				// We're going to cheat a little bit and store the color as a 3-component vector.
				// Code re-use!
				Vec3 finalColor(0,0,0);

				// For now, we'll assume the density has one homogenous color... White.
				Vec3 densityColor(1,1,1);
				Vec3 lightPosition = cameraPosition;// (0,1,0);// (1, 1, 1)

				// Distance along the ray. All rays start at the camera.
				for(float s=0.f; s<marchDistance; s += ds) {
					Vec3 rayPosition = cameraPosition + s * rayDirection;

					// Sample the density from a grid. Later, we'll play with how to render
					// things other than just grids.
					float density = volumeData->read(rayPosition);

					// // Compute the amount of light that reaches this point.
					// // We'll sample the light twice as coarsely as the main integral for now.
					// float lightValue = sampleLighting(rayPosition, lightPosition, ds);

					// // Hint at the future... We can play with the light absorption. (You can ignore this if you want.)
					// lightValue = powf(lightValue, 3.f);

					// // Let's also boost the amount of light so that everything is brighter.
					// lightValue *= 5;

					// Numerical integration of the path integral. Theory will be covered in
					// tutorial two.
					float dT = expf(density * -ds * kappa);
					T *= dT;
					// finalColor += ((1.f-dT)*T/kappa)*densityColor*lightValue;
					// finalColor += ((1.f-dT)*T/kappa)*densityColor;
					finalColor += ((1.f-dT)*T/kappa)*densityColor;
				}

				// Write out the output of our ray march into the image buffer.
				// We'll composite this image on top of some other color,
				// just to demonstrate
				// light blue color
				Vec3 backgroundColor(157./255., 204./255., 224./255.);//(0,0,0);//  (157, 204, 224);
				finalColor = (1.f-T)*finalColor + T*backgroundColor;

				for(int c=0;c<3;++c) {
					image[pndx + c] = finalColor[c];
				}

				// For the purposes of this tutorial, the final alpha for this image will be 1.
				image[pndx+3] = 1.f;
			}

			// Print out some progress information
			#pragma omp critical 
			{
				printf("\rRender progress: %.02f%%", 100.f*pi++/(width-1));
				fflush(stdout);
			}
	}

	printf("\n");
}

float VolumeRenderer::sampleLighting(const Vec3& x, const Vec3& lightPosition, float stepSize) {
	Vec3 lightDirection = lightPosition - x;
	float lightDistance = lightDirection.norm();

	// Normalize the direction vector that we will now march along.
	lightDirection *= 1.f/lightDistance;

	float densitySum = 0.f;
	for(float s=0; s<lightDistance; s+=stepSize) {
		Vec3 samplePosition = x + s * lightDirection;
		densitySum += volumeData->read(	samplePosition );
	}

	return exp(-stepSize * 1.f * densitySum);
}

void VolumeRenderer::writeImage(const char *path) {
	// // We'll use ImageMagick's c++ bindings here to make life way simpler.
	// // This gives us support for PNGs, JPEGs, BMP, TGA, etc for free.
	Magick::Image output;
	output.read(width,height,"RGBA",Magick::FloatPixel,image);
	output.write(path);

	// // stbi
	// // stbi_write_png("output.png", width, height, channels, data, width * channels);
	// const int channels=4;// rgba
	// printf("write image: %s\n", path);
	// stbi_write_png(path, width, height, channels, image, width * channels);
}

