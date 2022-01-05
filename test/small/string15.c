int main() {
  char * NT p = "garbage"; 
  p += 3; 
  return *p != 'b';
}
