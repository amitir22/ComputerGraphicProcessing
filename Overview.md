// Detailed Rendering Pipeline Steps:
// 1. Load Mesh Data: Import vertices, normals, faces, and optionally, texture coordinates from .obj file.
// 2. Transform Vertices to World Space: Apply model matrix to vertices to position the model in the scene.
// 3. Camera View Transformation: Transform vertices from world space to view (camera) space.
// 4. Back-Face Culling: Discard faces whose normals face away from the camera to reduce processing.
//    a. Calculate Face Normal: Use cross product of two edges of the face.
//    b. Determine Visibility: Dot product of face normal and vector from camera to face; cull if result > 0.
// 5. Projection Transformation: Project the 3D vertices in view space to 2D screen space.
//    a. Apply Perspective Divide: Convert 3D coordinates to normalized device coordinates (NDC).
//    b. Viewport Transformation: Map NDC to screen coordinates.
// 6. Clipping: Clip primitives against screen boundaries in NDC.
// 7. Scan Conversion/Rasterization: Convert projected 2D primitives to pixels on the screen.
//    a. Calculate Bounding Box in Screen Space: Optimize rasterization by processing relevant pixels only.
//    b. Edge Function Evaluation: Determine which pixels inside the bounding box are part of the primitive.
// 8. Fragment Processing: For each pixel, compute final color based on various factors.
//    a. Depth Testing: Compare fragment's depth with depth buffer to decide visibility.
//    b. Shading: Calculate color based on lighting model (e.g., Phong, Gouraud).
//    c. Texture Mapping: Apply textures if available, using interpolated UV coordinates.
//    d. Alpha Blending: Combine fragment's color with background color based on alpha value.
// 9. Display: Write the computed colors to the framebuffer for display on the screen.
