/* standard headers */
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <float.h>
#include <time.h>

/* SDL2 headers */
#include <SDL2/SDL.h>

/* common project headers */
#include "types.h"
#include "vec.h"
#include "ray_trace.h"

/* constants */
#define ASPECT_RATIO (1.0f)
enum { WIDTH = 600, HEIGHT = (i32)(WIDTH / ASPECT_RATIO) };
enum { SAMPLES_PER_PIXEL = 1000 };

/* main project headers */
#include "ray.h"
#include "camera.h"
#include "texture.h"
#include "aabb.h"
#include "hittable.h"
#include "material.h"
#include "hittable_list.h"
#include "sphere.h"
#include "moving_sphere.h"
#include "aarect.h"
#include "box.h"
#include "constanst_medium.h"
#include "bvh.h"

internal float3 ray_color(ray const *r, float3 background,  hittable const *world, i32 depth)
{
	hit_record rec;

	/* if we have exceeded the ray bounce limit, no more light is gathered. */
	if(depth <= 0)
		return 0.0f;
	
	/* if we hit something */
    if (!world->vtable->hit(world, r, 0.001f, FLT_MAX, &rec)) 
		return background;
	
	ray scattered;
	float3 emitted = rec.mat_ptr->vtable->emitted(rec.mat_ptr, rec.uv, rec.p);
	float3 attenuation;

	if(!rec.mat_ptr->vtable->scatter(rec.mat_ptr, r, &rec, &attenuation, &scattered))
		return emitted;

	return emitted + attenuation * ray_color(&scattered, background, world, depth - 1);
}

internal hittable *random_scene(void)
{
	hittable *world = new(hittable_list);

	/* preallocate memory for the world */
	hittable_list_reserve(world, 22 * 22);

	texture *checker = new_checker_texture((float3){0.2f, 0.3f, 0.1f}, 0.9f);
	material *material_ground = new_lambertian(checker);
	hittable_list_add(world, new(sphere, (float3){0, -1000, -1}, 1000, material_ground));

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
					hittable_list_add(world, new(sphere, center, 0.2f, sphere_material));
				}
				/* metal */
				else if(choose_mat < 0.95f)
				{
					float3 albedo = randomf3(0.5f, 1.0f);
					float fuzz = randomf(0.0f, 0.5f);
					material *sphere_material = new_metal(albedo, fuzz);
					hittable_list_add(world, new(sphere, center, 0.2f, sphere_material));
				}
				/* glass */
				else
				{
					material *sphere_material = new_dielectric(1.5f);
					hittable_list_add(world, new(sphere, center, 0.2f, sphere_material));
				}
			}
		}
	}
	material *material1 = new_dielectric(1.5f);
	hittable_list_add(world, new(sphere, (float3){0, 1, 0}, 1.0f, material1));

	material *material2 = new_lambertian((float3){0.4f, 0.2f, 0.1f});
	hittable_list_add(world, new(sphere, (float3){-4, 1, 0}, 1.0f, material2));

	material *material3 = new_metal((float3){0.7f, 0.6f, 0.6f}, 0.0f);
	hittable_list_add(world, new(sphere, (float3){4, 1, 0}, 1.0f, material3));
	world = new_bvh_node(world, 0, 0);
		
	return world;
}

internal hittable *two_spheres(void)
{
    hittable *objects = new(hittable_list);
    texture *checker = new_checker_texture((float3){0.2f, 0.3f, 0.1f}, 0.9f);

    hittable_list_add(objects, new(sphere, (float3){0, -10, 0}, 10, new_lambertian(checker)));
    hittable_list_add(objects, new(sphere, (float3){0, 10, 0}, 10, new_lambertian(checker)));

    return objects;
}

internal hittable *two_perlin_spheres(void)
{
    hittable *objects = new(hittable_list);

    texture *pertext = new_noise_texture(4);
    hittable_list_add(objects, new(sphere, (float3){0, -1000, 0}, 1000, new_lambertian(pertext)));
    hittable_list_add(objects, new(sphere, (float3){0, 2, 0}, 2, new_lambertian(pertext)));

    return objects;
}

internal hittable *simple_light(void)
{
    hittable *objects = new(hittable_list);

    texture *pertext = new_noise_texture(4);
    hittable_list_add(objects, new(sphere, (float3){0, -1000, 0}, 1000, new_lambertian(pertext)));
    hittable_list_add(objects, new(sphere, (float3){0, 2, 0}, 2, new_dielectric(1.4f)));
    material *difflight = new_diffuse_light(4);
    hittable_list_add(objects, new_xy_rect(3, 5, 1, 3, -2, difflight));
    hittable_list_add(objects, new(sphere, (float3){0, 5, 0}, 1.5f, difflight));


    return objects;
}

internal hittable *cornell_box(void)
{
    hittable *objects = new(hittable_list);

    material *red = new_lambertian((float3){0.65f, 0.05f, 0.05f});
    material *white = new_lambertian(.73f);
    material *green = new_lambertian((float3){0.12f, 0.45f, 0.15f});
    material *light = new_diffuse_light((float3){15, 15, 15});

    /* add walls and light */
    hittable_list_add(objects, new_yz_rect(0, 555, 0, 555, 555, green));
    hittable_list_add(objects, new_yz_rect(0, 555, 0, 555,  0, red));
    hittable_list_add(objects, new_xz_rect(113, 443, 127, 432, 554, light));
    hittable_list_add(objects, new_xz_rect(0, 555, 0, 555,  0, white));
    hittable_list_add(objects, new_xz_rect(0, 555, 0, 555, 555, white));
    hittable_list_add(objects, new_xy_rect(0, 555, 0, 555, 555, white));

    /* add box1 */
	hittable *box1 = new(box, 0, (float3){165, 330, 165}, white);
    box1 = new(rotate_y, box1, 15);
    box1 = new(translate, box1, (float3){265, 0, 295});
	hittable_list_add(objects, box1);
    
	/* add box2 */
	hittable *box2 = new(box, 0, 165, white);
    box2 = new(rotate_y, box2, -18);
    box2 = new(translate, box2, (float3){130, 0, 65});
	hittable_list_add(objects, box2);
    
	
    return objects;
}

internal hittable *cornell_smoke(void)
{
    hittable *objects = new(hittable_list);

    material *red = new_lambertian((float3){0.65f, 0.05f, 0.05f});
    material *white = new_lambertian(.73f);
    material *green = new_lambertian((float3){0.12f, 0.45f, 0.15f});
    material *light = new_diffuse_light((float3){7, 7, 7});

    /* add walls and light */
    hittable_list_add(objects, new_yz_rect(0, 555, 0, 555, 555, green));
    hittable_list_add(objects, new_yz_rect(0, 555, 0, 555,  0, red));
    hittable_list_add(objects, new_xz_rect(113, 443, 127, 432, 554, light));
    hittable_list_add(objects, new_xz_rect(0, 555, 0, 555,  0, white));
    hittable_list_add(objects, new_xz_rect(0, 555, 0, 555, 555, white));
    hittable_list_add(objects, new_xy_rect(0, 555, 0, 555, 555, white));

    /* create box1 */
    hittable *box1 = new(box, 0, (float3){165, 330, 165}, white);
    box1 = new(rotate_y, box1, 15);
    box1 = new(translate, box1, (float3){265, 0, 295});

    /* create box2 */
    hittable *box2 = new(box, 0, 165, white);
    box2 = new(rotate_y, box2, -18);
    box2 = new(translate, box2, (float3){130, 0, 65});

    /* add boxes */
    hittable_list_add(objects, new(constant_medium, box1, 0.01f, 0.0f));
    hittable_list_add(objects, new(constant_medium, box2, 0.01f, 1.0f));

    return objects;
}

internal hittable *final_scene(void)
{
    hittable *boxes1 = new(hittable_list);
    material *ground = new_lambertian((float3){0.48f, 0.83f, 0.53f});

    i32 const boxes_per_side = 20;
    hittable_list_reserve(boxes1, boxes_per_side * boxes_per_side);
    for(i32 i = 0; i < boxes_per_side; ++i)
    {
        for (i32 j = 0; j < boxes_per_side; ++j)
        {
            float w = 100.0f;
            float x0 = -1000.0f + i * w;
            float z0 = -1000.0f + j * w;
            float y0 = 0.0f;
            float x1 = x0 + w;
            float y1 = randomf(1, 101);
            float z1 = z0 + w;

            hittable_list_add(boxes1, new(box, (float3){x0, y0, z0}, (float3){x1, y1, z1}, ground));
        }
    }

    hittable *objects = new(hittable_list, new_bvh_node(boxes1, 0, 1));

    material *light = new_diffuse_light((float3){7, 7, 7});
    hittable_list_add(objects, new_xz_rect(123, 423, 147, 412, 554, light));

    float3 center1 = {400, 400, 200};
    float3 center2 = center1 + (float3){30, 0, 0};
    material *moving_sphere_material = new_lambertian((float3){0.7f, 0.3f, 0.1f});
    hittable_list_add(objects, new_moving_sphere(center1, center2, 0, 1, 50, moving_sphere_material));

    hittable_list_add(objects, new(sphere, (float3){260, 150, 45}, 50, new_dielectric(1.5f)));
    hittable_list_add(objects, new(sphere, (float3){0, 150, 145}, 50, new_metal((float3){0.8f, 0.8f, 0.9f}, 10.0f)));

    hittable *boundary = new(sphere, (float3){360, 150, 145}, 70, new_dielectric(1.5f));
    hittable_list_add(objects, boundary);
    hittable_list_add(objects, new(constant_medium, boundary, 0.2f, (float3){0.2f, 0.4f, 0.9f}));
    boundary = new(sphere, 0, 5000, new_dielectric(1.5f));
    hittable_list_add(objects, new(constant_medium, boundary, 0.0001f, (float3){1, 1, 1}));

    material *emat = new_lambertian(new_solid_color((float3){0, 0, 0.75f}));
    hittable_list_add(objects, new(sphere, (float3){400, 200, 400}, 100, emat));
    texture *pertext = new_noise_texture(0.1f);
    hittable_list_add(objects, new(sphere, (float3){220, 280, 300}, 80, new_lambertian(pertext)));

    hittable *boxes2 = new(hittable_list);
    material *white = new_lambertian(.73f);
    i32 const ns = 1000;
    hittable_list_reserve(boxes2, ns);
    for(i32 j = 0; j < ns; ++j)
    {
        hittable_list_add(boxes2, new(sphere, randomf3(0, 165), 10, white));
    }

    hittable_list_add(objects, new(translate, new(rotate_y, new_bvh_node(boxes2, 0.0f, 1.0f), 15), (float3){-100, 270, 395}));

    return objects;
}

internal void generate_image(uint32_t *image)
{
	hittable *world;
	float3 lookfrom;
	float3 lookat;
	float aperture;
	float vfov;
	float3 background;
	
	/* choose the scene */
	switch (7)
    {
		case 1:
			world = random_scene();
			background = (float3){0.70f, 0.80f, 1};
			lookfrom = (float3){13, 2, 3};
			lookat = (float3){0, 0, 0};
			vfov = 20.0f;
			aperture = 0.0f;
		    break;
        case 2:
            world = two_spheres();
			background = (float3){0.70f, 0.80f, 1};
            lookfrom = (float3){13, 2, 3};
            lookat = 0;
            vfov = 20.0f;
            aperture = 0.0f;
            break;
        case 3:
            world = two_perlin_spheres();
            background = (float3){0.70f, 0.80f, 1};
            lookfrom = (float3){13, 2, 3};
            lookat = 0;
            vfov = 20.0f;
            aperture = 0.0f;
            break;
        case 4:
        	world = simple_light();
        	background = 0;
        	lookfrom = (float3){26, 3, 6};
        	lookat = (float3){0, 2, 0};
        	vfov = 20.0f;
        	aperture = 0.0f;
        	break;
        case 5:
            world = cornell_box();
            background = 0;
            lookfrom = (float3){278, 278, -800};
            lookat = (float3){278, 278, 0};
            vfov = 40.0f;
            aperture = 0.0f;
            break;
        case 6:
            world = cornell_smoke();
            background = 0;
            lookfrom = (float3){278, 278, -800};
            lookat = (float3){278, 278, 0};
            vfov = 40.0f;
            aperture = 0.0f;
            break;
        default:
        case 7:
            world = final_scene();
            background = 0;
            lookfrom = (float3){478, 278, -600};
            lookat = (float3){278, 278, 0};
            vfov = 40.0f;
            aperture = 0.0f;
            break;
	}

	/* setup camera */
    float3 vup = (float3){0, 1, 0};
    float dist_to_focus = 10;
    camera cam = make_camera(lookfrom, lookat, vup, vfov, ASPECT_RATIO, aperture, dist_to_focus, 0, 1.0f);
		
	
	/* render */
	#pragma omp parallel for
	for(i32 pix = 0; pix < HEIGHT * WIDTH; ++pix) /* pixel index */
	{	
		float3 pixel_color = {0};
		for(i32 s = 0; s < SAMPLES_PER_PIXEL; ++s)
		{
			/* get pixel coords */
			int32_t i = pix % WIDTH;
			int32_t j = (HEIGHT - 1) - (pix / WIDTH);
		
			float u = (i + randomf())/(float)(WIDTH - 1);
			float v = (j + randomf())/(float)(HEIGHT - 1);
			ray r = camera_get_ray(&cam, u, v);

			/* add to the samples */
			{
				float3 result_color = ray_color(&r, background, world, 50);
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
	uint32_t *image = malloc(sizeof(uint32_t) * WIDTH * HEIGHT);

	/* generate the image and time how long it took */
	clock_t start_time = clock();
	generate_image(image);		
	clock_t end_time = clock();

	printf("it took %f seconds", (float)(end_time - start_time) / (float)CLOCKS_PER_SEC);

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
