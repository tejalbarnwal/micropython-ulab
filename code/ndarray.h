
/*
 * This file is part of the micropython-ulab project, 
 *
 * https://github.com/v923z/micropython-ulab
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2020 Zoltán Vörös
*/

#ifndef _NDARRAY_
#define _NDARRAY_

#include "py/objarray.h"
#include "py/binary.h"
#include "py/objstr.h"
#include "py/objlist.h"

#define ULAB_MAX_DIMS	            4
#define NDARRAY_PRINT_THRESHOLD  	10
#define NDARRAY_PRINT_EDGEITEMS		3

#if MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_FLOAT
#define FLOAT_TYPECODE 'f'
#elif MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_DOUBLE
#define FLOAT_TYPECODE 'd'
#endif

// this typedef is lifted from objfloat.c, because mp_obj_float_t is not exposed
typedef struct _mp_obj_float_t {
    mp_obj_base_t base;
    mp_float_t value;
} mp_obj_float_t;

#ifdef OPENMV
#define mp_obj_is_bool(o) (MP_OBJ_IS_TYPE((o), &mp_type_bool))
#define translate(x) x

typedef struct _mp_obj_slice_t {
    mp_obj_base_t base;
    mp_obj_t start;
    mp_obj_t stop;
    mp_obj_t step;
} mp_obj_slice_t;

void mp_obj_slice_get(mp_obj_t self_in, mp_obj_t *, mp_obj_t *, mp_obj_t *);
#else
#if CIRCUITPY
#define mp_obj_is_bool(o) (MP_OBJ_IS_TYPE((o), &mp_type_bool))
#define mp_obj_is_int(x) (MP_OBJ_IS_INT((x)))
#else
#define translate(x) MP_ERROR_TEXT(x)
#endif
#endif

#define SWAP(t, a, b) { t tmp = a; a = b; b = tmp; }

#define NDARRAY_NUMERIC   0
#define NDARRAY_BOOLEAN   1

#define NDARRAY_NDARRAY_TYPE    1
#define NDARRAY_ITERABLE_TYPE   2

extern const mp_obj_type_t ulab_ndarray_type;

enum NDARRAY_TYPE {
    NDARRAY_BOOL = '?', // this must never be assigned to the dtype!
    NDARRAY_UINT8 = 'B',
    NDARRAY_INT8 = 'b',
    NDARRAY_UINT16 = 'H', 
    NDARRAY_INT16 = 'h',
    NDARRAY_FLOAT = FLOAT_TYPECODE,
};

typedef struct _ndarray_obj_t {
	mp_obj_base_t base;
    uint8_t dense;
    uint8_t dtype;
    uint8_t itemsize;
	uint8_t boolean;
	uint8_t ndim;
    size_t len;
	size_t shape[ULAB_MAX_DIMS];
	int32_t strides[ULAB_MAX_DIMS];
    void *array;
} ndarray_obj_t;

// this is a helper structure, so that we can return shape AND strides from a function
typedef struct _ndarray_header_obj_t {
    size_t *shape;
    int32_t *strides;
    int8_t axis;
} ndarray_header_obj_t;

typedef struct _short_descriptor_t {
	size_t left_shape[ULAB_MAX_DIMS];
	size_t right_shape[ULAB_MAX_DIMS];
	size_t output_shape[ULAB_MAX_DIMS];
	int32_t left_strides[ULAB_MAX_DIMS];
	int32_t right_strides[ULAB_MAX_DIMS];
	int32_t output_strides[ULAB_MAX_DIMS];
	bool broadcastable;
} short_descriptor_t;

mp_obj_t ndarray_new_ndarray_iterator(mp_obj_t , size_t , mp_obj_iter_buf_t *);

mp_float_t ndarray_get_float_value(void *, uint8_t , size_t );
bool ndarray_object_is_nditerable(mp_obj_t );
void fill_array_iterable(mp_float_t *, mp_obj_t );


void ndarray_print(const mp_print_t *, mp_obj_t , mp_print_kind_t );

mp_obj_t ndarray_set_printoptions(size_t , const mp_obj_t *, mp_map_t *);
MP_DECLARE_CONST_FUN_OBJ_KW(ndarray_set_printoptions_obj);

mp_obj_t ndarray_get_printoptions(void);
MP_DECLARE_CONST_FUN_OBJ_0(ndarray_get_printoptions_obj);

void ndarray_print_row(const mp_print_t *, ndarray_obj_t *, size_t , size_t );
void ndarray_assign_elements(ndarray_obj_t *, mp_obj_t , uint8_t , size_t *);
size_t *ndarray_new_coords(uint8_t );
size_t *ndarray_contract_shape(ndarray_obj_t *, uint8_t );
int32_t *ndarray_contract_strides(ndarray_obj_t *, uint8_t );

ndarray_obj_t *ndarray_new_dense_ndarray(uint8_t , size_t *, uint8_t );
ndarray_obj_t *ndarray_new_ndarray_from_tuple(mp_obj_tuple_t *, uint8_t );
ndarray_obj_t *ndarray_new_ndarray(uint8_t , size_t *, int32_t *, uint8_t );
ndarray_obj_t *ndarray_new_linear_array(size_t , uint8_t );
bool ndarray_is_dense(ndarray_obj_t *);

mp_obj_t ndarray_copy(mp_obj_t );
#ifdef CIRCUITPY
mp_obj_t ndarray_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *args, mp_map_t *kw_args);
#else
mp_obj_t ndarray_make_new(const mp_obj_type_t *, size_t , size_t , const mp_obj_t *);
#endif
mp_obj_t ndarray_subscr(mp_obj_t , mp_obj_t , mp_obj_t );
mp_obj_t ndarray_getiter(mp_obj_t , mp_obj_iter_buf_t *);
mp_obj_t ndarray_binary_op(mp_binary_op_t , mp_obj_t , mp_obj_t );
mp_obj_t ndarray_unary_op(mp_unary_op_t , mp_obj_t );

mp_obj_t ndarray_shape(mp_obj_t );
MP_DECLARE_CONST_FUN_OBJ_1(ndarray_shape_obj);

mp_obj_t ndarray_strides(mp_obj_t );
MP_DECLARE_CONST_FUN_OBJ_1(ndarray_strides_obj);

mp_obj_t ndarray_size(mp_obj_t );

mp_obj_t ndarray_itemsize(mp_obj_t );
MP_DECLARE_CONST_FUN_OBJ_1(ndarray_itemsize_obj);

mp_obj_t ndarray_itemsize(mp_obj_t );
mp_obj_t ndarray_flatten(size_t , const mp_obj_t *, mp_map_t *);

mp_obj_t ndarray_reshape(mp_obj_t , mp_obj_t );
MP_DECLARE_CONST_FUN_OBJ_2(ndarray_reshape_obj);

mp_obj_t ndarray_transpose(mp_obj_t );
MP_DECLARE_CONST_FUN_OBJ_1(ndarray_transpose_obj);

mp_int_t ndarray_get_buffer(mp_obj_t obj, mp_buffer_info_t *bufinfo, mp_uint_t flags);
//void ndarray_attributes(mp_obj_t , qstr , mp_obj_t *);

ndarray_obj_t *ndarray_from_mp_obj(mp_obj_t );

/*  
    mp_obj_t row = mp_obj_new_list(n, NULL);
    mp_obj_list_t *row_ptr = MP_OBJ_TO_PTR(row);
    
    should work outside the loop, but it doesn't. Go figure! 
*/

#define NDARRAY_INDEX_FROM_FLAT2(ndarray, stride_array, shape_strides, index, _tindex, _nindex) do {\
    size_t Q;\
    (_tindex) = (index);\
    (_nindex) = (ndarray)->offset;\
    for(size_t _x=0; _x < (ndarray)->ndim; _x++) {\
        Q = (_tindex) / (shape_strides)[_x];\
        (_tindex) -= Q * (shape_strides)[_x];\
        (_nindex) += Q * (stride_array)[_x];\
    }\
} while(0)

/*
#define RUN_BINARY_LOOP(typecode, type_out, type_left, type_right, ol, or, op, m, n, len, linc, rinc) do {\
    type_left *left = (type_left *)(ol)->array->items;\
    type_right *right = (type_right *)(or)->array->items;\
    if(((op) == MP_BINARY_OP_ADD) || ((op) == MP_BINARY_OP_SUBTRACT) || ((op) == MP_BINARY_OP_MULTIPLY) || ((op) == MP_BINARY_OP_POWER)) {\
        ndarray_obj_t *out = create_new_ndarray((m), (n), (typecode));\
        type_out *(odata) = (type_out *)out->array->items;\
        if((op) == MP_BINARY_OP_ADD) { for(size_t i=0; i < (len); i++, left+=linc, right+=rinc) *odata++ = *left + *right; }\
		else if((op) == MP_BINARY_OP_MULTIPLY) { for(size_t i=0; i < (len); i++, left+=linc, right+=rinc) *odata++ = *left * *right; }\
        else if((op) == MP_BINARY_OP_POWER) { for(size_t i=0; i < (len); i++, left+=linc, right+=rinc) *odata++ = (type_out)MICROPY_FLOAT_C_FUN(pow)(*left, *right); }\
        else if((op) == MP_BINARY_OP_SUBTRACT) { for(size_t i=0; i < (len); i++, left+=linc, right+=rinc) *odata++ = *left - *right; }\
        return MP_OBJ_FROM_PTR(out);\
	} else if((op) == MP_BINARY_OP_TRUE_DIVIDE) {\
        ndarray_obj_t *out = create_new_ndarray((m), (n), NDARRAY_FLOAT);\
        mp_float_t *odata = (mp_float_t *)out->array->items;\
        for(size_t i=0; i < (len); i++, left+=linc, right+=rinc) {*odata++ = (mp_float_t)(*left)/(mp_float_t)(*right);}\
        return MP_OBJ_FROM_PTR(out);\
	} else if(((op) == MP_BINARY_OP_LESS) || ((op) == MP_BINARY_OP_LESS_EQUAL) ||  \
             ((op) == MP_BINARY_OP_MORE) || ((op) == MP_BINARY_OP_MORE_EQUAL) || \
             ((op) == MP_BINARY_OP_EQUAL) || ((op) == MP_BINARY_OP_NOT_EQUAL)) {\
        mp_obj_t out_list = mp_obj_new_list(0, NULL);\
        for(size_t i=0; i < m; i++) {\
            mp_obj_t row = mp_obj_new_list(n, NULL);\
            mp_obj_list_t *row_ptr = MP_OBJ_TO_PTR(row);\
			if((op) == MP_BINARY_OP_LESS) { for(size_t j=0; j < n; j++, left+=linc, right+=rinc) row_ptr->items[j] = *left < *right ? mp_const_true : mp_const_false; }\
			else if((op) == MP_BINARY_OP_LESS_EQUAL) { for(size_t j=0; j < n; j++, left+=linc, right+=rinc) row_ptr->items[j] = *left <= *right ? mp_const_true : mp_const_false; }\
			else if((op) == MP_BINARY_OP_MORE) { for(size_t j=0; j < n; j++, left+=linc, right+=rinc) row_ptr->items[j] = *left > *right ? mp_const_true : mp_const_false; }\
			else if((op) == MP_BINARY_OP_MORE_EQUAL) { for(size_t j=0; j < n; j++, left+=linc, right+=rinc) row_ptr->items[j] = *left >= *right ? mp_const_true : mp_const_false; }\
			else if((op) == MP_BINARY_OP_EQUAL) { for(size_t j=0; j < n; j++, left+=linc, right+=rinc) row_ptr->items[j] = *left == *right ? mp_const_true : mp_const_false; }\
			else if((op) == MP_BINARY_OP_NOT_EQUAL) { for(size_t j=0; j < n; j++, left+=linc, right+=rinc) row_ptr->items[j] = *left != *right ? mp_const_true : mp_const_false; }\
            if(m == 1) return row;\
            mp_obj_list_append(out_list, row);\
		}\
        return out_list;\
    }\
} while(0)

*/
#endif
