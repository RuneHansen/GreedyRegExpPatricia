


aNFAnode* aNFAnodeConstructor() {
  aNFAnode* ret = (aNFAnode*) malloc(sizeof(aNFAnode));
  ret->nr = 0;
  ret->input = '\0';
  ret->charClass.nranges = 0;
  ret->left = NULL;
  ret->right = NULL;
  return ret;
}


void aNFAgen(BitC_Regex* E, aNFAnode* i, aNFAnode* f, std::string* language) {
  if (E) {
    switch(E->op) {
    case BitC_RegexOp_Lit :
      {
      std::cout << "Laver lit\n";
      i->input = E->litChar;
      i->left = f;
      if(language->find(i->input) == std::string::npos) {
        *language += i->input;
      }
      break;
      }
    case BitC_RegexOp_LitString :
      {
      std::cout << "Laver litstring\n";
      aNFAnode* prev = aNFAnodeConstructor();
      i->left = prev;
      prev->left = f;
      prev->input = E->litString[0];
      if(language->find(prev->input) == std::string::npos) {
        *language += prev->input;
      }
      for(int i = 1; E->litString[i] != '\0'; i++) {
        aNFAnode* cur = aNFAnodeConstructor();
        prev->left = cur;
        cur->input = E->litString[i];
        cur->left = f;
        prev = cur;
        if(language->find(prev->input) == std::string::npos) {
          *language += prev->input;
        }
      }
      break;
      }
    case BitC_RegexOp_Plus :
      {
      std::cout << "Laver plus\n";
      aNFAnode* loop = aNFAnodeConstructor();
      aNFAnode* q = aNFAnodeConstructor();
      i->left = q;
      aNFAgen(E->sub1, q, loop, language);
      loop->left = q;
      loop->right = f;
      break;
      }
    case BitC_RegexOp_Star :
      {
      std::cout << "Laver star\n";
      aNFAnode* loop = aNFAnodeConstructor();
      aNFAnode* q = aNFAnodeConstructor();
      i->left = loop;
      loop->left = q;
      loop->right = f;
      aNFAgen(E->sub1,q,loop, language);
      break;
      }
    /*case BitC_RegexOp_Concat2 :
      {
      aNFAnode* q = (aNFAnode*) malloc (sizeof(aNFAnode));
      aNFAgen(E->subs[0],i,q);
      aNFAgen(E->subs[1],q,f);
      break;
      } */
    case BitC_RegexOp_Concat :
    {
      std::cout << "Laver concat\n";
      aNFAnode* prev = aNFAnodeConstructor();
      aNFAgen(E->subs[0],i,prev, language);
      for(int i = 1; i < E->nsub; i++) {
        aNFAnode* cur = aNFAnodeConstructor();
        aNFAgen(E->subs[i],prev,cur, language);
        cur->left = f;
        prev = cur;
      }
      break;
    }
    case BitC_RegexOp_Alt:
      {
      std::cout << "Laver alt\n";
      aNFAnode* s = aNFAnodeConstructor();
      aNFAnode* q = aNFAnodeConstructor();
      aNFAnode* s2;
      i->left = q;
      i->right = s;
      aNFAgen(E->subs[0], q, f, language);

      int n = E->nsub;
      //std::cout << "nsub number " << n << std::endl;
      //std::cout << "Nr's " << s->nr << " " << q->nr << " " << i->nr << " " << f->nr << "\n";
      for(int j = 1; j < n; j++) {
        s2 = aNFAnodeConstructor();
        q = aNFAnodeConstructor();
        s->left = q;
        //std::cout << "q's nr " << q->nr << std::endl;
        aNFAgen(E->subs[j], q, f, language);
        //std::cout << "Nr2's" << q->nr << " " << s2->nr << std::endl;
        if(j == n-1) {
          free(s2);
        } else {
          s->right = s2;
          s = s2;
        }
      }
      break;
      }
    case BitC_RegexOp_Any:
      {
      std::cout << "Laver any\n";
      i->charClass.nranges = (size_t) -1;
      i->left = f;
      break;
      }
    case BitC_RegexOp_BeginText:
      std::cout << "Lavede BeginText\n";
      break;
    case BitC_RegexOp_Capture:
      {
      std::cout << "Laver capture\n";

      aNFAgen(E->sub1, i, f, language);
      break;
      }
    case BitC_RegexOp_CharClass:
      {
        std::cout << "Laver charClass\n";
        for(int i = 0; i < E->charClass.nranges; i++) {
          for(int j = (int) E->charClass.ranges[i].from; j <= (int) E->charClass.ranges[i].to; j++) {
            if(language->find( (char) j ) == std::string::npos) {
              *language += (char) j;
            }
          }
        }
        i->charClass.ranges = (BitC_CharRange*) malloc(sizeof(BitC_CharRange) * E->charClass.nranges);
        for(int j = 0; j < E->charClass.nranges; j++) {
          i->charClass.ranges[j].to = E->charClass.ranges[j].to;
          i->charClass.ranges[j].from = E->charClass.ranges[j].from;
        }
        i->charClass.nranges = E->charClass.nranges;
        i->charClass.inclusive = E->charClass.inclusive;
        i->left = f;
        break;
      }
    case BitC_RegexOp_EndText:
      {
      std::cout << "Lavede EndText " << E->nsub << std::endl;
      aNFAgen(E->sub1, i, f, language);
      break;
      }
    case BitC_RegexOp_Question:
      {
      std::cout << "Laver Question\n";
      i->right = f;
      aNFAnode* q = aNFAnodeConstructor();
      i->left = q;
      aNFAgen(E->sub1, q, f, language);
      break;
      }
    case BitC_RegexOp_Repeat:
      std::cout << "Lavede Repeat\n";
      break;
    case BitC_RegexOp_Unit:
      {
      std::cout << "Laver unit\n";
      i->left = f;
      break;
      }
    default:
      {
      std::cout << "Hit default, " << E->op << std::endl;
      }
    }
  }
}
