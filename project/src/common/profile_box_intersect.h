void measure(int runs, ray r, hittable* hittable, const char* impl, const char* ray_type) {
  hit_record rec;
  auto start_time = steady_clock::now();
  for (int x = 0; x < runs; x++) {
    hittable->hit(r, 0.001, infinity, rec);
  }
  auto end_time = steady_clock::now();
  double elapsed_seconds = std::chrono::duration_cast<seconds>(end_time - start_time).count();
  std::cerr << impl << " " << ray_type << " took " << elapsed_seconds << " runs(" << runs << ")\n";
}


void profile_box_intersect() {
  material* aluminum = new metal(color(0.8, 0.85, 0.88), 0.0);
  hittable* ob = new obox(point3(0,0,0), point3(165,330,165), aluminum);
  hittable* nb = new box(point3(0,0,0), point3(165,330,165), aluminum);
  
  // first generate a series of hit and miss rays
  hit_record rec;
  std::vector<ray> hit;
  std::vector<ray> miss;
  std::vector<ray> dubious;
  for (int j = 0; j < 200; ++j) {
      for (int i = 0; i < 200; ++i) {
          for (int s = 0; s < samples_per_pixel; ++s) {
              auto u = (i + 0.0) / (image_width-1);
              auto v = (j + 0.0) / (image_height-1);
              ray r = cam->get_ray_r(u, v);
              bool nh = nb->hit(r, 0.001, infinity, rec);
              bool oh = ob->hit(r, 0.001, infinity, rec);
              if (nh && oh) {
                hit.push_back(r);
              } else if (!nh && !oh) {
                miss.push_back(r);
              } else {
                dubious.push_back(r);
              }
          }
      }
  }
  
  
  std::cerr << "hit size = " << hit.size() << "\n";
  std::cerr << "miss size = " << miss.size() << "\n";
  std::cerr << "dubious size = " << dubious.size() << "\n";
  auto runs = 2147483647;
  auto r_hit = hit[100];
  auto r_miss = miss[100];
  
  // measurements
  measure(runs, r_hit,  nb, " box", "hit ");
  measure(runs, r_hit,  ob, "obox", "hit ");

  measure(runs, r_miss, nb, " box", "miss");
  measure(runs, r_miss, ob, "obox", "miss");
}