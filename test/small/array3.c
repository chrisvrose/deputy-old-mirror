#define NDIM 3
double distv(double * COUNT(NDIM) v)
{
  // So we could increment it
  double *w = v;
  return *w;
}

int main() {
  double v[NDIM] = { 1.0, 2.0, 3.0 };
  if (1.0 != distv(v)) {
    return 1;
  }
  return 0;
}
