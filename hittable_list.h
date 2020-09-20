#pragma once
typedef struct hittable_list
{
	hittable parent;
	struct
	{
		size_t size;
		size_t capacity;
		hittable **data;
	} objects;
} hittable_list;

internal void hittable_list_reserve(void *self, size_t size)
{
	hittable_list *this = self;

	this->objects.capacity = size;
	this->objects.data = realloc(this->objects.data, size * sizeof(hittable*));
}

internal void hittable_list_add(void *self, void *object)
{
	hittable_list *this = self;
	
	/* if we have not already allocate memory */
	if(NULL == this->objects.data)
	{
		this->objects.data = malloc(sizeof(hittable*));
		this->objects.data[0] = object;
		this->objects.size = 1;
		this->objects.capacity = 1;
	}
	else
	{
		if(++this->objects.size > this->objects.capacity)
		{
			this->objects.capacity = this->objects.size * 2;
			this->objects.data = realloc(this->objects.data, sizeof(hittable*) * this->objects.capacity);
		}

		this->objects.data[this->objects.size - 1] = object;
	}
}

internal bool hittable_list_hit(hittable_list const *this, ray const *r, float t_min, float t_max, hit_record *rec)
{
	hit_record temp_rec;
	bool hit_anything = false;
	float closest_so_far = t_max;

	for(size_t i = 0; i < this->objects.size; ++i)
	{
		if(this->objects.data[i]->vtable->hit(this->objects.data[i], r, t_min, closest_so_far, &temp_rec))
		{
			hit_anything = true;
			closest_so_far = temp_rec.t;
			*rec = temp_rec;
		}
	}

	return hit_anything;
}

bool hittable_list_bounding_box(hittable_list const *this, float t0, float t1, aabb *output_box)
{
	if(this->objects.size == 0) return false;

	aabb temp_box;
	bool first_box = true;

	for(size_t i = 0; i < this->objects.size; ++i)
	{
		if(!this->objects.data[i]->vtable->bounding_box(this->objects.data[i], t0, t1, &temp_box)) return false;
		*output_box = first_box ? temp_box : surrounding_box(*output_box, temp_box);
		first_box = false;		
	}

	return false;
}


__attribute__((overloadable)) internal hittable *new_hittable_list(void)
{
	hittable_list *result = calloc(1, sizeof(hittable_list));
	static struct hittable_vtable hittable_list_vtable = { 
		.hit = (void*)&hittable_list_hit,
		.bounding_box = (void*)&hittable_list_bounding_box
	};
	result->parent.vtable = &hittable_list_vtable;
	return (void*)result;		
}

__attribute__((overloadable)) internal hittable *new_hittable_list(void *object)
{
	hittable_list *result = (void*)new_hittable_list();
	hittable_list_add(result, object);
	return (void*)result;		
}
