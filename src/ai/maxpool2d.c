/*
  Copyright (C) 2018 Paul Brossier <piem@aubio.org>

  This file is part of aubio.

  aubio is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  aubio is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with aubio.  If not, see <http://www.gnu.org/licenses/>.

*/



#include "aubio_priv.h"
#include "fmat.h"
#include "tensor.h"
#include "maxpool2d.h"

struct _aubio_maxpool2d_t {
  uint_t pool_size[2];
  uint_t stride[2];
};

static void aubio_maxpool2d_debug(aubio_maxpool2d_t *c,
    aubio_tensor_t *input_tensor);

aubio_maxpool2d_t *new_aubio_maxpool2d(uint_t pool_size[2])
{
  aubio_maxpool2d_t *c = AUBIO_NEW(aubio_maxpool2d_t);

  AUBIO_GOTO_FAILURE((sint_t)pool_size[0] > 0);
  AUBIO_GOTO_FAILURE((sint_t)pool_size[1] > 0);

  c->pool_size[0] = pool_size[0];
  c->pool_size[1] = pool_size[1];

  c->stride[0] = 1;
  c->stride[1] = 1;

  return c;

failure:
  del_aubio_maxpool2d(c);
  return NULL;
}

void del_aubio_maxpool2d(aubio_maxpool2d_t* c) {
  AUBIO_ASSERT(c);
  AUBIO_FREE(c);
}

void aubio_maxpool2d_debug(aubio_maxpool2d_t *c, aubio_tensor_t *input_tensor)
{
  AUBIO_DBG("maxpool2d: input (%d, %d, %d) ¤ maxpool2d (pool_size = (%d, %d)) ->"
      " (%d, %d, %d) (no params)\n",
      input_tensor->shape[0],
      input_tensor->shape[1],
      input_tensor->shape[2],
      c->pool_size[0],
      c->pool_size[1],
      input_tensor->shape[0] / c->pool_size[0],
      input_tensor->shape[1] / c->pool_size[1],
      input_tensor->shape[2]);
}

uint_t aubio_maxpool2d_get_output_shape(aubio_maxpool2d_t *c,
    aubio_tensor_t *input, uint_t *shape)
{
  AUBIO_ASSERT(c);
  AUBIO_ASSERT(shape && sizeof(shape) == 2*sizeof(uint_t));
  AUBIO_ASSERT(input);
  shape[0] = input->shape[0] / c->pool_size[0];
  shape[1] = input->shape[1] / c->pool_size[1];
  shape[2] = input->shape[2];

  aubio_maxpool2d_debug(c, input);

  return AUBIO_OK;
}

void aubio_maxpool2d_do(aubio_maxpool2d_t *c, aubio_tensor_t *input_tensor,
    aubio_tensor_t *output_tensor)
{
  uint_t i, j, k, a, b;
  AUBIO_ASSERT(c && input_tensor && output_tensor);

  //aubio_maxpool2d_debug(c, input_tensor);

  for (i = 0; i < output_tensor->shape[0]; i++)
  {
    for (j = 0; j < output_tensor->shape[1]; j++)
    {
      for (k = 0; k < output_tensor->shape[2]; k++)
      {
        smpl_t m = -10000.;
        for (a = 0; a < c->pool_size[0]; a++) {
          for (b = 0; b < c->pool_size[1]; b++) {
            uint_t idx = (j * c->pool_size[1] + b) * input_tensor->shape[2] + k;
            m = MAX(m, input_tensor->data[i * c->pool_size[0] + a][idx]);
          }
        }
        output_tensor->data[i][j * output_tensor->shape[2] + k] = m;
      }
    }
  }
}

void aubio_maxpool2d_train(aubio_maxpool2d_t *c,
    aubio_tensor_t *backprop)
{
  AUBIO_ASSERT(c && backprop);
}