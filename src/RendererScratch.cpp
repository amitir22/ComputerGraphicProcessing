//#include "Renderer.h" 
//
//#include <chrono>
//
//#include "geometry2.h"
//
////Vec3f EigenVec3ToGeometryVec3(const vec3 &v) {
////    return Vec3f(v.x(), v.y(), v.z());
////}
////
////Matrix44f EigenMat4ToGeometryMat4(mat4 m) {
////    return Matrix44f(
////        m(0, 0), m(0, 1), m(0, 2), m(0, 3),
////        m(1, 0), m(1, 1), m(1, 2), m(1, 3),
////        m(2, 0), m(2, 1), m(2, 2), m(2, 3),
////        m(3, 0), m(3, 1), m(3, 2), m(3, 3)
////    );
////}
//
//float min3(const float &a, const float &b, const float &c)
//{ return std::min(a, std::min(b, c)); }
//
//float max3(const float &a, const float &b, const float &c)
//{ return std::max(a, std::max(b, c)); }
//
//float edgeFunction(const Vec3f &a, const Vec3f &b, const Vec3f &c)
//{
//    float t1 = c[0] - a[0];
//    float t2 = b[1] - a[1];
//    float t3 = c[1] - a[1];
//    float t4 = b[0] - a[0];
//     return (c[0] - a[0]) * (b[1] - a[1]) - (c[1] - a[1]) * (b[0] - a[0]); 
//}
//
//void ProjectFaceToRasterScratch(const Face& face, const mat4& mvp, const mat4& view_port_transform, Vec3f& v0Raster, Vec3f& v1Raster, Vec3f& v2Raster)
//{	
//    vec4 h_vertex0 = homogoneizeVector(face.vertices[0].position_in_local_space);
//    h_vertex0 = mvp * h_vertex0;
//    h_vertex0 /= h_vertex0.w();
//    h_vertex0 = view_port_transform * h_vertex0;
//    v0Raster = Vec3f(h_vertex0.x(), h_vertex0.y(), h_vertex0.z());
//    
//    vec4 h_vertex1 = homogoneizeVector(face.vertices[1].position_in_local_space);
//    h_vertex1 = mvp * h_vertex1;
//    h_vertex1 /= h_vertex1.w();
//    h_vertex1 = view_port_transform * h_vertex1;
//    v1Raster = Vec3f(h_vertex1.x(), h_vertex1.y(), h_vertex1.z());
//
//    vec4 h_vertex2 = homogoneizeVector(face.vertices[2].position_in_local_space);
//    h_vertex2 = mvp * h_vertex2;
//    h_vertex2 /= h_vertex2.w();
//    h_vertex2 = view_port_transform * h_vertex2;
//    v2Raster = Vec3f(h_vertex2.x(), h_vertex2.y(), h_vertex2.z());
//}
//
//void Renderer::RenderFaceScratch(vec3 v0, vec3 v1, vec3 v2, vec3 n0, vec3 n1, vec3 n2, vec3 face_normal) {
//    // TODO Backface culling
//    vec3 forward = scene_->GetActiveCamera()->forward;
//    vec3 face_normal = face.normal_;
//    float z_dot = scene_->GetActiveCamera()->forward.dot(face.normal_);
//    //std::cout << "z_dot: " << z_dot << std::endl;
//
//    if (z_dot > 0)
//    {
//        return;
//    }
//    // convert face vertices to Vec3f
//    // const Vec3f &v0 = EigenVec3ToGeometryVec3(face.vertices[0].position_in_local_space);
//    // const Vec3f &v1 = EigenVec3ToGeometryVec3(face.vertices[1].position_in_local_space);
//    // const Vec3f &v2 = EigenVec3ToGeometryVec3(face.vertices[2].position_in_local_space);
//
//    // const Matrix44f worldToCamera = EigenMat4ToGeometryMat4(view_transform_ * model_transform_);
//
//    const float l = -right;
//    const float r = right;
//    const float t = top;
//    const float b = -top;
//    const float nearClippingPLane = static_cast<float>(z_near);
//    const float farClippingPLane = static_cast<float>(z_far);
//    const uint32_t imageWidth = width_;
//    const uint32_t imageHeight = height_;
//    
//    // auto t_start = std::chrono::high_resolution_clock::now();
//    Vec3f v0Raster, v1Raster, v2Raster;
//    ProjectFaceToRasterScratch(face, mvp_, view_port_transform_, v0Raster, v1Raster, v2Raster);
//    // if all vertices share same y value - return 
//    if (v0Raster.y == v1Raster.y && v0Raster.y == v2Raster.y) return;
//    // convertToRaster(v0, worldToCamera, l, r, t, b, nearClippingPLane, imageWidth, imageHeight, v0Raster);
//    // convertToRaster(v1, worldToCamera, l, r, t, b, nearClippingPLane, imageWidth, imageHeight, v1Raster);
//    // convertToRaster(v2, worldToCamera, l, r, t, b, nearClippingPLane, imageWidth, imageHeight, v2Raster);
//
//    v0Raster.z = 1 / v0Raster.z,
//    v1Raster.z = 1 / v1Raster.z,
//    v2Raster.z = 1 / v2Raster.z;
//
//    float xmin = min3(v0Raster.x, v1Raster.x, v2Raster.x);
//    float ymin = min3(v0Raster.y, v1Raster.y, v2Raster.y);
//    float xmax = max3(v0Raster.x, v1Raster.x, v2Raster.x);
//    float ymax = max3(v0Raster.y, v1Raster.y, v2Raster.y);
//    
//    // the triangle is out of screen
//    if (xmin > imageWidth - 1 || xmax < 0 || ymin > imageHeight - 1 || ymax < 0)    return;
//
//    // be careful xmin/xmax/ymin/ymax can be negative. Don't cast to uint32_t
//    uint32_t x0 = std::max(int32_t(0), (int32_t)(std::floor(xmin)));
//    uint32_t x1 = std::min(int32_t(imageWidth) - 1, (int32_t)(std::floor(xmax)));
//    uint32_t y0 = std::max(int32_t(0), (int32_t)(std::floor(ymin)));
//    uint32_t y1 = std::min(int32_t(imageHeight) - 1, (int32_t)(std::floor(ymax)));
//
//    float area = edgeFunction(v0Raster, v1Raster, v2Raster);
//
//    for (uint32_t y = y0; y <= y1; ++y) 
//    {
//        for (uint32_t x = x0; x <= x1; ++x) {
//            Vec3f pixelSample(x + 0.5, y + 0.5, 0);
//            float w0 = edgeFunction(v1Raster, v2Raster, pixelSample);
//            float w1 = edgeFunction(v2Raster, v0Raster, pixelSample);
//            float w2 = edgeFunction(v0Raster, v1Raster, pixelSample);
//            bool inside_ = (w0 >= 0 && w1 >= 0 && w2 >= 0);
//            if (area < 0)
//                inside_ = (w0 <= 0 && w1 <= 0 && w2 <= 0);
//            if (inside_) {
//                w0 /= area;
//                w1 /= area;
//                w2 /= area;
//                float oneOverZ = v0Raster.z * w0 + v1Raster.z * w1 + v2Raster.z * w2;
//                float z = 1 / oneOverZ;
//                // Depth-buffer test
//                if (z < z_buffer_[y * imageWidth + x]) {
//                    z_buffer_[y * imageWidth + x] = z;
//                    // TODO fragment shader - compute color, according to selected lighting method
//                    // Renderer::getShadingForFragment(Face, fragment)
//                    DrawPixel(x, y);
//                } // end if depth-buffer test
//            } // end if inside
//        } // end for x
//    } // end for y
//}
//
//// Renderer::getShadingForFragment(Face, fragment)
