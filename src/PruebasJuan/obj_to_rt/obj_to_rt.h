#ifndef OBJ_TO_RT_H
# define OBJ_TO_RT_H

# include <stddef.h>
# include "../../../include/vec3.h"

typedef struct s_face
{
    int	v[3];
    int	vt[3];
    int	vn[3];
}	t_face;

typedef struct s_mesh
{
    t_vec3	*vertices;
    size_t	v_count;
    size_t	v_cap;
    t_vec3	*normals;
    size_t	vn_count;
    size_t	vn_cap;
    t_vec3	*texcoords;
    size_t	vt_count;
    size_t	vt_cap;
    t_face	*faces;
    size_t	f_count;
    size_t	f_cap;
}	t_mesh;

#endif