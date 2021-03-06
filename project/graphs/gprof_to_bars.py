import re
import matplotlib.pyplot as plt
import numpy as np

text = """
 16.75     11.57    11.57 996121811     0.00     0.00  xz_rect::hit(ray const&, double, double, hit_record&) const
 13.00     20.54     8.98 365608358     0.00     0.00  hittable_list::hit(ray const&, double, double, hit_record&) const
 11.87     28.73     8.19 731216716     0.00     0.00  yz_rect::hit(ray const&, double, double, hit_record&) const
  8.39     34.52     5.79 548412537     0.00     0.00  xy_rect::hit(ray const&, double, double, hit_record&) const
  8.24     40.21     5.69 264905095     0.00     0.00  sphere::hit(ray const&, double, double, hit_record&) const
  7.94     45.69     5.48     -        -         -     ray_color(ray const&, vec3 const&, hittable const&, std::shared_ptr<hittable>, int)
  5.33     49.37     3.68 102618288     0.00     0.00  onb::build_from_w(vec3 const&)
  3.98     52.12     2.75 50110139     0.00     0.00  dielectric::scatter(ray const&, hit_record const&, scatter_record&) const
  3.91     54.82     2.70 82100916     0.00     0.00  lambertian::scattering_pdf(ray const&, hit_record const&, ray const&) const
  2.27     56.39     1.57 82100916     0.00     0.00  cosine_pdf::value(vec3 const&) const
  1.59     58.77     1.10 182804179     0.00     0.00  rotate_y::hit(ray const&, double, double, hit_record&) const
  1.45     59.77     1.00 41067036     0.00     0.00  cosine_pdf::generate() const
  1.39     60.73     0.96 14595143     0.00     0.00  metal::scatter(ray const&, hit_record const&, scatter_record&) const
  1.35     61.66     0.93 82100916     0.00     0.00  hittable_list::pdf_value(vec3 const&, vec3 const&) const
  1.16     62.47     0.80 82100916     0.00     0.00  sphere::pdf_value(vec3 const&, vec3 const&) const
  1.13     63.25     0.78 182804179     0.00     0.00  translate::hit(ray const&, double, double, hit_record&) const
  1.07     63.99     0.74 82100916     0.00     0.00  xz_rect::pdf_value(vec3 const&, vec3 const&) const
  0.94     64.64     0.65 41033880     0.00     0.00  hittable_list::random(vec3 const&) const
  0.87     65.24     0.60 82100916     0.00     0.00  lambertian::scatter(ray const&, hit_record const&, scatter_record&) const
  0.57     65.63     0.39 20517372     0.00     0.00  sphere::random(vec3 const&) const
  0.52     65.99     0.36 182804179     0.00     0.00  flip_face::hit(ray const&, double, double, hit_record&) const
  0.46     66.31     0.32 182804179     0.00     0.00  box::hit(ray const&, double, double, hit_record&) const
  0.36     66.56     0.25 20516508     0.00     0.00  xz_rect::random(vec3 const&) const
  0.33     66.79     0.23      -        -         -   stbi__gif_info_raw(stbi__context*, int*, int*, int*)
  0.28     67.20     0.19 96273425     0.00     0.00  solid_color::value(double, double, vec3 const&) const
  0.28     67.39     0.19 82100916     0.00     0.00  hittable_pdf::value(vec3 const&) const
  0.23     67.92     0.16      -        -         -   hittable::random(vec3 const&) const
  0.17     68.04     0.12 146806198     0.00     0.00  material::emitted(ray const&, hit_record const&, double, double, vec3 const&) const
  0.17     68.16     0.12 41033880     0.00     0.00  hittable_pdf::generate() const
  0.16     68.27     0.11      -        -         -   hittable_pdf::~hittable_pdf()
  0.13     68.36     0.09 82100916     0.00     0.00  hittable_pdf::~hittable_pdf()
  0.12     68.52     0.08     -        -         -    mixture_pdf::value(vec3 const&) const
  0.12     68.60     0.08     -        -         -    frame_dummy
  0.10     68.67     0.07        1    70.01   100.01  rotate_y::rotate_y(std::shared_ptr<hittable>, double)
  0.09     68.73     0.06    -        -          -   mixture_pdf::generate() const
  0.07     68.78     0.05 82100916     0.00     0.00  cosine_pdf::~cosine_pdf()
  0.07     68.88     0.05 17984605     0.00     0.00  diffuse_light::emitted(ray const&, hit_record const&, double, double, vec3 const&) const
  0.04     68.94     0.03        1    30.00    30.00  box::bounding_box(double, double, aabb&) const
  0.04     68.97     0.03    -        -         -     stbi_info_from_callbacks
  0.03     68.99     0.02    -        -         -     write_color(std::ostream&, vec3, int)
  0.01     69.03     0.01    -        -         -     sphere::bounding_box(double, double, aabb&) const
  0.00     69.03     0.00 17984605     0.00     0.00  material::scatter(ray const&, hit_record const&, scatter_record&) const
  0.00     69.03     0.00        1     0.00     0.00  box::box(vec3 const&, vec3 const&, std::shared_ptr<material>)
"""


lines = [x.strip().split(maxsplit=6) for x in text.split("\n") if x.strip() != ""]

functions = [f'{x[6].split("(")[0]} ({x[3]})' for x in lines]
totalTime = [float(x[0]) for x in lines]
x_pos = np.arange(len(functions))

plt.bar(x_pos,totalTime)
plt.xticks(x_pos, functions, rotation=90)
plt.subplots_adjust(bottom=0.4, top=0.99)
plt.ylabel('Time spent %')
plt.xlabel('Function (call count)')

plt.show()
