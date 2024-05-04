#include "link.h"

//#define SERIAL_DEBUG

struct MyLink *init_link(){
#ifdef SERIAL_DEBUG
    Serial.println("init_link");
#endif
    struct MyLink *p = (struct MyLink *)malloc(sizeof(struct MyLink));
    p->next = NULL;
    p->anchor_addr = 0;
    p->range[0] = 0.0;
    p->range[1] = 0.0;
    p->range[2] = 0.0;

    return p;
}

void add_link(struct MyLink *p, uint16_t addr){
#ifdef SERIAL_DEBUG
    Serial.println("add_link");
#endif
    struct MyLink *temp = p;
    //Find struct MyLink end
    while (temp->next != NULL)
    {
        temp = temp->next;
    }

    Serial.println("add_link:find struct MyLink end");
    //Create a anchor
    struct MyLink *a = (struct MyLink *)malloc(sizeof(struct MyLink));
    a->anchor_addr = addr;
    a->range[0] = 0.0;
    a->range[1] = 0.0;
    a->range[2] = 0.0;
    a->dbm = 0.0;
    a->next = NULL;

    //Add anchor to end of struct MyLink
    temp->next = a;

    return;
}

struct MyLink *find_link(struct MyLink *p, uint16_t addr){
#ifdef SERIAL_DEBUG
    Serial.println("find_link");
#endif
    if (addr == 0)
    {
        Serial.println("find_link:Input addr is 0");
        return NULL;
    }

    if (p->next == NULL)
    {
        Serial.println("find_link:Link is empty");
        return NULL;
    }

    struct MyLink *temp = p;
    //Find target struct MyLink or struct MyLink end
    while (temp->next != NULL)
    {
        temp = temp->next;
        if (temp->anchor_addr == addr)
        {
            // Serial.println("find_link:Find addr");
            return temp;
        }
    }

    Serial.println("find_link:Can't find addr");
    return NULL;
}

bool isLinkEmpty(struct MyLink *p){
  if (p->next == NULL)
    {
        Serial.println("find_link:Link is empty");
        return true;
    }
    return false;
}

void fresh_link(struct MyLink *p, uint16_t addr, float range, float dbm){
#ifdef SERIAL_DEBUG
    Serial.println("fresh_link");
#endif
    struct MyLink *temp = find_link(p, addr);
    if (temp != NULL)
    {
        temp->range[2] = temp->range[1];
        temp->range[1] = temp->range[0];

        temp->range[0] = (range + temp->range[1] + temp->range[2]) / 3;
        temp->dbm = dbm;
        return;
    }
    else
    {
        Serial.println("fresh_link:Fresh fail");
        return;
    }
}

void print_link(struct MyLink *p){
#ifdef SERIAL_DEBUG
    Serial.println("print_link");
#endif
    struct MyLink *temp = p;

    while (temp->next != NULL)
    {
        //Serial.println("Dev %d:%d m", temp->next->anchor_addr, temp->next->range);
        Serial.println(temp->next->anchor_addr, HEX);
        Serial.println(temp->next->range[0]);
        Serial.println(temp->next->dbm);
        temp = temp->next;
    }

    return;
}

void delete_link(struct MyLink *p, uint16_t addr){
#ifdef SERIAL_DEBUG
    Serial.println("delete_link");
#endif
    if (addr == 0)
        return;

    struct MyLink *temp = p;
    while (temp->next != NULL)
    {
        if (temp->next->anchor_addr == addr)
        {
            struct MyLink *del = temp->next;
            temp->next = del->next;
            free(del);
            return;
        }
        temp = temp->next;
    }
    return;
}

void make_link_json(struct MyLink *p, String *s){
#ifdef SERIAL_DEBUG
    Serial.println("make_link_json");
#endif
    *s = "{\"links\":[";
    struct MyLink *temp = p;

    while (temp->next != NULL)
    {
        temp = temp->next;
        char link_json[50];
        sprintf(link_json, "{\"A\":\"%X\",\"R\":\"%.1f\"}", temp->anchor_addr, temp->range[0]);
        *s += link_json;
        if (temp->next != NULL)
        {
            *s += ",";
        }
    }
    *s += "]}";
    Serial.println(*s);
}

void make_link_http_post(struct MyLink *p, struct UWB_ANCHOR *p2){
  struct MyLink *temp = p;
  struct UWB_ANCHOR *uwb_anchor = p2;

  uwb_anchor->Anchor_0 = 0.0;
  uwb_anchor->Anchor_1 = 0.0;
  uwb_anchor->Anchor_2 = 0.0;
  uwb_anchor->Anchor_3 = 0.0;
  uwb_anchor->Anchor_4 = 0.0;
  uwb_anchor->Anchor_5 = 0.0;
  uwb_anchor->Anchor_6 = 0.0;
  uwb_anchor->Anchor_7 = 0.0;
  
  while(temp->next != NULL){
    temp = temp->next;

    if(temp->anchor_addr == uwb_anchor->ANCHOR_DEC_ADDR[0])
      uwb_anchor->Anchor_0 = temp->range[0];
    else if(temp->anchor_addr == uwb_anchor->ANCHOR_DEC_ADDR[1])
      uwb_anchor->Anchor_1 = temp->range[0];
    else if(temp->anchor_addr == uwb_anchor->ANCHOR_DEC_ADDR[2])
      uwb_anchor->Anchor_2 = temp->range[0];
    else if(temp->anchor_addr == uwb_anchor->ANCHOR_DEC_ADDR[3])
      uwb_anchor->Anchor_3 = temp->range[0];
    else if(temp->anchor_addr == uwb_anchor->ANCHOR_DEC_ADDR[4])
      uwb_anchor->Anchor_4 = temp->range[0];
    else if(temp->anchor_addr == uwb_anchor->ANCHOR_DEC_ADDR[5])
      uwb_anchor->Anchor_5 = temp->range[0];
    else if(temp->anchor_addr == uwb_anchor->ANCHOR_DEC_ADDR[6])
      uwb_anchor->Anchor_6 = temp->range[0];
    else if(temp->anchor_addr == uwb_anchor->ANCHOR_DEC_ADDR[7])
      uwb_anchor->Anchor_7 = temp->range[0];
  }
//  Serial.println("In Function Make Link HTTP POST Data;");
//  Serial.println("Anchor_0 -> " + String(uwb_anchor->Anchor_0));
//  Serial.println("Anchor_1 -> " + String(uwb_anchor->Anchor_1));
//  Serial.println("Anchor_2 -> " + String(uwb_anchor->Anchor_2));
//  Serial.println("Anchor_3 -> " + String(uwb_anchor->Anchor_3));
//  Serial.println("Anchor_4 -> " + String(uwb_anchor->Anchor_4));
//  Serial.println("Anchor_5 -> " + String(uwb_anchor->Anchor_5));
//  Serial.println("Anchor_6 -> " + String(uwb_anchor->Anchor_6));
//  Serial.println("Anchor_7 -> " + String(uwb_anchor->Anchor_7));
  
}
