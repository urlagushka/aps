class circle_t
{
  public:
    circle_t(int k);

    void method1(int k = 0);
    void method2(int k = 0);
    ...

  private:
    int __k;
};

void circle_t::method1(int k = 0)
{
  int to_use = __k;
  if (k != 0)
  {
    to_use = k;
  }

  // algorithm
}