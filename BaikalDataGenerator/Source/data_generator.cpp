/**********************************************************************
Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
********************************************************************/

#include "data_generator.h"

#include "render.h"
#include "utils.h"

#include "Rpr/WrapObject/LightObject.h"
#include "Rpr/WrapObject/SceneObject.h"


DataGeneratorResult GenerateDataset(DataGeneratorParams const* params)
try
{
    std::filesystem::path output_dir = params->output_dir;

    if (!exists(output_dir))
    {
        std::filesystem::create_directory(output_dir);
    }
    else if (!std::filesystem::is_directory(output_dir))
    {
        return kDataGeneratorBadOutputDir;
    }

    auto* scene = SceneObject::Cast<SceneObject>(params->scene);
    if (scene == nullptr)
    {
        return kDataGeneratorBadScene;
    }

    Render render(scene,
                  params->width,
                  params->height,
                  params->bounces_num,
                  params->device_idx);

    for (size_t i = 0; i < params->lights_num; ++i)
    {
        auto* light = LightObject::Cast<LightObject>(params->lights[i]);
        if (light == nullptr)
        {
            return kDataGeneratorBadLight;
        }
        render.AttachLight(light);
    }

    if (params->spp_num == 0)
    {
        return kDataGeneratorBadSpp;
    }

    std::vector<size_t> sorted_spp(params->spp, params->spp + params->spp_num);
    std::sort(sorted_spp.begin(), sorted_spp.end());

    sorted_spp.erase(std::unique(sorted_spp.begin(), sorted_spp.end()), sorted_spp.end());

    if (sorted_spp.front() <= 0)
    {
        return kDataGeneratorBadSpp;
    }

    unsigned camera_end_idx = params->cameras_start_idx + params->cameras_num - 1;

    render.SaveMetadata(output_dir,
                        params->scene_name,
                        params->cameras_start_idx,
                        camera_end_idx,
                        params->cameras_offset_idx,
                        params->gamma_correction != 0);

    for (unsigned i = params->cameras_start_idx; i <= camera_end_idx; ++i)
    {
        auto* camera = CameraObject::Cast<CameraObject>(params->cameras[i]);
        if (camera == nullptr)
        {
            return kDataGeneratorBadCamera;
        }
        render.GenerateSample(camera,
                              params->cameras_offset_idx + i,
                              sorted_spp,
                              output_dir,
                              params->gamma_correction != 0);
        if (params->progress_callback)
        {
            params->progress_callback(params->cameras_offset_idx + i);
        }
    }

    return kDataGeneratorSuccess;
}
catch (...)
{
    return kDataGeneratorUnknownError;
}