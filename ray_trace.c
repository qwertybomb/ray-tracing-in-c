/* standard headers */
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <float.h>

/* SDL2 headers */
#include <SDL2/SDL.h>

/* project headers */
#include "types.h"
#include "vec.h"
#include "ray_trace.h"

/* constants */
#define ASPECT_RATIO (3.0f/ 2.0f)
enum { WIDTH = 500, HEIGHT = (i32)(WIDTH / ASPECT_RATIO) };
enum { SAMPLES_PER_PIXEL = 25 };

/* main project headers */
#include "ray.h"
#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"
/* TODO: figure out how render triangles without floating point errors */
#include "material.h"

internal float3 ray_color(ray const *r, hittable const *world, i32 depth)
{
	hit_record rec;

	/* if we have exceeded the ray bounce limit, no more light is gathered. */
	if(depth <= 0)
		return 0.0f;
	
	/* if we hit something */
    if (world->vtable->hit(world, r, 0.001f, FLT_MAX, &rec)) 
	{
		ray scattered;
		float3 attenuation;
		if(rec.mat_ptr->vtable->scatter(rec.mat_ptr, r, &rec, &attenuation, &scattered))
		{
			return attenuation * ray_color(&scattered, world, depth - 1);
		}
		return 0.0f;
    }  
	 	
	float3 unit_direction = normalize(r->dir);
    float t = 0.5f*(unit_direction.y + 1.0f);
    return (1.0f-t)*(float3){1.0f, 1.0f, 1.0f} + t*(float3){0.5f, 0.7f, 1.0f};	
}

internal hittable *random_scene(void)
{
	hittable *world = new_hittable_list();

	material *material_ground = new_lambertian((float3){0.5f, 0.5f, 0.5f});
	hittable_list_add(world, new_sphere((float3){0, -1000, -1}, 1000, material_ground));

	for(i32 a = -11; a < 11; ++a)
	{
		for(i32 b = -11; b < 11; ++b)
		{
			float choose_mat = randomf();
			float3 center = {a + 0.9f * randomf(), 0.2f, b + 0.9f * randomf()};

			if(length(center - (float3){4, 0.2f, 0}) > 0.9f)
			{
				/* diffuse */
				if(choose_mat < 0.8f)
				{
					float3 albedo = randomf3() * randomf3();
					material *sphere_material = new_lambertian(albedo);
					hittable_list_add(world, new_sphere(center, 0.2f, sphere_material));
				}
				/* metal */
				else if(choose_mat < 0.95f)
				{
					float3 albedo = randomf3(0.5f, 1.0f);
					float fuzz = randomf(0.0f, 0.5f);
					material *sphere_material = new_metal(albedo, fuzz);
					hittable_list_add(world, new_sphere(center, 0.2f, sphere_material));
				}
				/* glass */
				else
				{
					material *sphere_material = new_dielectric(1.5f);
					hittable_list_add(world, new_sphere(center, 0.2f, sphere_material));
				}
			}
		}
	}

	material *material1 = new_dielectric(1.5f);
	hittable_list_add(world, new_sphere((float3){0, 1, 0}, 1.0f, material1));

	material *material2 = new_lambertian((float3){0.4f, 0.2f, 0.1f});
	hittable_list_add(world, new_sphere((float3){-4, 1, 0}, 1.0f, material2));

	material *material3 = new_metal((float3){0.7f, 0.6f, 0.6f}, 0.0f);
	hittable_list_add(world, new_sphere((float3){4, 1, 0}, 1.0f, material3));
	
	return world;
}

internal void generate_image(u32 *image)
{
	/* setup world */
	hittable *world = random_scene();
			
	/* setup camera */
	float3 lookfrom = {13, 2, 3};
	float3 lookat = {0, 0, 0};
	float3 vup = {0, 1, 0};
	float dist_to_focus = 10.0f;
	float aperture = 0.0f;
	
	camera cam = make_camera(lookfrom, lookat, vup, 20, ASPECT_RATIO, aperture, dist_to_focus);

	/* render */
	#pragma omp parallel for
	for(i32 pix = 0; pix < HEIGHT * WIDTH; ++pix) /* pixel index */
	{	
		float3 pixel_color = {0};
		for(i32 s = 0; s < SAMPLES_PER_PIXEL; ++s)
		{
			/* get pixel coords */
			i32 i = pix % WIDTH;
			i32 j = (HEIGHT - 1) - (pix / WIDTH);
		
			float u = (i + randomf())/(float)(WIDTH - 1);
			float v = (j + randomf())/(float)(HEIGHT - 1);
			ray r = camera_get_ray(&cam, u, v);

			/* add to the samples */
			{
				float3 result_color = ray_color(&r, world, 50);
				pixel_color += result_color;	
			}
		}

		write_pixel(&image[pix], pixel_color, SAMPLES_PER_PIXEL);
	}
}

int main(int argc, char *argv[])
{
	/* we don't need these
	 * but SDL2 needs main to always have the signature of int main(int, char*[]);
	 */
	(void)argc;
	(void)argv;

	/* setup SDL2 */
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		fprintf(stderr, "Error: with SDL2 initialization \"%s\"\n", SDL_GetError());
		return -1;
	}

	SDL_Window *window = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
	if(NULL == window)
	{
		fprintf(stderr, "Error: with SDL2 window creation \"%s\"\n", SDL_GetError());
		return -2;
	}

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if(NULL == renderer)
	{
		fprintf(stderr, "Error: with SDL2 renderer creation \"%s\"\n", SDL_GetError());
		return -3;	
	}

	SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, WIDTH, HEIGHT);
	if(NULL == texture)
	{
		fprintf(stderr, "Error: with SDL2 texture creation \"%s\"\n", SDL_GetError());
		return -4;	
	}

	/* allocate memory for an image */
	u32 *image = malloc(sizeof(u32) * WIDTH * HEIGHT);

	/* generate image */
	generate_image(image);		

	/*  main loop */
	bool quit = false;
	while(!quit)
	{
					/* check for a quit event */
		{
			SDL_Event event = { 0 };
			if(SDL_PollEvent(&event))
				switch (event.type)
				{
					case SDL_QUIT:
					{
						quit = true;
					} break;
				}
		}
		/* draw texture */
		{
			SDL_UpdateTexture(texture, NULL, image, sizeof(u32) * WIDTH);
			SDL_RenderCopy(renderer, texture, NULL, NULL);
			SDL_RenderPresent(renderer);
		}
	}

	/* success */
	return 0;
}
