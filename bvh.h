#pragma once

typedef struct bvh_node
{
	hittable parent;
	hittable *left;
	hittable *right;
	aabb box;
} bvh_node;

bool bvh_node_bounding_box(bvh_node const *this, float t0, float t1, aabb *output_box)
{
	*output_box = this->box;
	return true;
}

internal bool bvh_node_hit(bvh_node const *this, ray const *r, float t_min, float t_max, hit_record *rec)
{
	if(!aabb_hit(&this->box, r, t_min, t_max))
		return false;

	bool hit_left = this->left->vtable->hit(this->left, r, t_min, t_max, rec);
	bool hit_right = this->right->vtable->hit(this->right, r, t_min, hit_left ? rec->t : t_max, rec);

	return hit_left || hit_right;	
}

internal inline int box_compare(hittable ** a, hittable ** b, i32 axis)
{
	aabb box_a;
	aabb box_b;

	if(!(*a)->vtable->bounding_box(*a, 0, 0, &box_a)
	|| !(*b)->vtable->bounding_box(*b, 0, 0, &box_b))
	{
			fprintf(stderr, "No bounding box in bvh_node constructor.\n");	
	}

	return box_a.min[axis] == box_b.min[axis] ? 0 : box_a.min[axis] < box_b.min[axis] ? 1 : -1;
}

internal int box_x_compare(hittable ** a, hittable ** b)
{
	return box_compare(a, b, 0);
}

internal int box_y_compare(hittable ** a, hittable ** b)
{
	return box_compare(a, b, 1);
}

internal int box_z_compare(hittable ** a, hittable ** b)
{
	return box_compare(a, b, 2);
}

__attribute__((overloadable)) internal hittable *new_bvh_node(hittable **objects, size_t start, size_t end, float time0, float time1)
{
	/* allocate memory for the result */
	bvh_node *result = calloc(sizeof(bvh_node), 1);
	
	i32 axis = (i32)randomf(0,3);
	/* stupid qsort */ int(*comparator)(hittable **, hittable **) = axis == 0 ? box_x_compare
															 								: axis == 1 ? box_y_compare
															 								: box_z_compare;
	size_t object_span = end - start;
	if(object_span == 1)
	{
		result->left = result->right = objects[start];
	}
	else if(object_span == 2)
	{
		if(comparator(&objects[start], &objects[start + 1]) == 1)
		{
			result->left = objects[start];
			result->right = objects[start + 1];
		}
		else
		{
			result->left = objects[start + 1];
			result->right = objects[start];
		}
	}
	else
	{
		qsort(objects + start, object_span, sizeof(hittable*), (void*)comparator);

		size_t mid = start + object_span / 2;
		result->left = new_bvh_node(objects, start, mid, time0, time1);
		result->right = new_bvh_node(objects, mid, end, time0, time1);
	}

	aabb box_left = {0}, box_right = {0};

	if(!result->left->vtable->bounding_box(result->left, time0, time1, &box_left)
	|| !result->right->vtable->bounding_box(result->right, time0, time1, &box_right))
	{
		fprintf(stderr, "No bounding box in bvh_node constructor.\n");	
	}
	result->box = surrounding_box(box_left, box_right);

	static struct hittable_vtable bvh_node_vtable = {
		.hit = (void*)&bvh_node_hit,
		.bounding_box = (void*)&bvh_node_bounding_box
	};
	result->parent.vtable = &bvh_node_vtable;
	
	return (void*)result;
}

__attribute__((overloadable)) internal hittable *new_bvh_node(hittable *list_in, float time0, float time1)
{
	hittable_list *list = (void*)list_in;
	return new_bvh_node(list->objects.data, 0, list->objects.size, time0, time1);	
}
