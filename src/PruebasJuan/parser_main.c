#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "Error\nUsage: %s <scene.rt>\n", argv[0]);
        return 2;
    }
    t_scene scene;
    t_parse_result pr = parse_scene(argv[1], &scene);
    if (!pr.ok) {
        if (pr.line > 0)
            fprintf(stderr, "Error\nParser:%s:%d: %s\n", argv[1], pr.line, pr.message ? pr.message : "");
        else
            fprintf(stderr, "Error\nParser:%s: %s\n", argv[1], pr.message ? pr.message : "");
        parse_result_free(&pr);
        return 1;
    }
    parse_result_free(&pr);

    // Print a compact summary of the parsed scene (useful for debugging)
    printf("OK\n");
    printf("A: ratio=%.3f color=(%.3f,%.3f,%.3f)\n", scene.ambient.ratio, scene.ambient.color.x, scene.ambient.color.y, scene.ambient.color.z);
    printf("C: pos=(%.3f,%.3f,%.3f) dir=(%.3f,%.3f,%.3f) fov=%.1f\n",
           scene.camera.pos.x, scene.camera.pos.y, scene.camera.pos.z,
           scene.camera.dir.x, scene.camera.dir.y, scene.camera.dir.z,
           scene.camera.fov_deg);
    printf("L: pos=(%.3f,%.3f,%.3f) bright=%.3f color=(%.3f,%.3f,%.3f)\n",
           scene.light.pos.x, scene.light.pos.y, scene.light.pos.z,
           scene.light.bright, scene.light.color.x, scene.light.color.y, scene.light.color.z);

    // Count objects
    int nsp=0, npl=0, ncy=0; for (t_object *o = scene.objects; o; o = o->next) {
        if (o->type == OBJ_SPHERE) nsp++; else if (o->type == OBJ_PLANE) npl++; else if (o->type == OBJ_CYLINDER) ncy++;
    }
    printf("Objects: sp=%d, pl=%d, cy=%d\n", nsp, npl, ncy);

    scene_free(&scene);
    return 0;
}
