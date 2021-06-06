

int  a = 1;
int c = 3;
int b = 2;
int d = 4;

int *ptr;

void setup()
{
  Serial.begin(115200);
  ptr = (int*)malloc(5 * sizeof(int));
//  int (*ptr)[5];
  int **p = getPtr();
}

void loop()
{



}

int **getPtr()
{
  int **dptr = &ptr;
  ptr = &a;
  dptr++
  *dptr = &b;
  dptr++;
  *dptr  = &c;
  return &ptr;
}

