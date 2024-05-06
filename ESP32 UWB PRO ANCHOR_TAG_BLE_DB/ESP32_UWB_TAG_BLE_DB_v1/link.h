#include <Arduino.h>

struct MyLink
{
    uint16_t anchor_addr;
    float range[3];
    float dbm;
    struct MyLink *next;
};

struct UWB_ANCHOR{
  double Anchor_0;
  double Anchor_1;
  double Anchor_2;
  double Anchor_3;
  double Anchor_4;
  double Anchor_5;
  double Anchor_6;
  double Anchor_7;
  byte ANCHOR_SAMPLE_RATE;
  uint16_t ANCHOR_DEC_ADDR[8];
};

struct MyLink *init_link();
void add_link(struct MyLink *p, uint16_t addr);
struct MyLink *find_link(struct MyLink *p, uint16_t addr);
void fresh_link(struct MyLink *p, uint16_t addr, float range, float dbm);
void print_link(struct MyLink *p);
void delete_link(struct MyLink *p, uint16_t addr);
void make_link_json(struct MyLink *p,String *s);
void make_link_http_post(struct MyLink *p, struct UWB_ANCHOR *p2);
bool isLinkEmpty(struct MyLink *p);
