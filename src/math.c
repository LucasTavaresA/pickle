static inline double clamp(double value, double min, double max)
{
  if (value < min)
  {
    return min;
  }
  else if (value > max)
  {
    return max;
  }
  else
  {
    return value;
  }
}
