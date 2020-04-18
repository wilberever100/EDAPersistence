// Copyright 2020 Roger Peralta Aranibar
#ifndef SOURCE_PARTIALDIRECTEDGRAPH_HPP_
#define SOURCE_PARTIALDIRECTEDGRAPH_HPP_

#include <utility>

#include "DirectedGraph.hpp"
using namespace std;
namespace ADE {
namespace Persistence {

template <typename TipoNodo, typename TDato>
class modi {
 public:
  TipoNodo* ptr;  //
  // TipoNodo** doublePointer;
  TDato value;
  unsigned int version;
  size_t index;
  bool puntero;
  modi(TDato val, int ver)
      : value(val),
        version(ver),
        puntero(false),
        ptr(nullptr) {}  // modifica valor del mismo nodo
  modi(size_t ind, TipoNodo* ptr_, unsigned int ver)  // ptr al nodo modificado
      : ptr( ptr_),
        //ptr(dynamic_cast < PartialNode<TDato> * (ptr_)),
        version(ver),
        puntero(true),
        value(-1),
        index(ind) {}  // modifica un forward
};
template <typename Type>
//template <typename Type,typename G>
class PartialNode : public Node<Type> {
 public:
  // typedef typename G::
  typedef Type data_type;
  std::vector<modi<PartialNode, data_type>*> mods;  // vector de modificaciones
  int version_;
  int usedMods;                     // modificaciones usadas hasta 2*in_ptrs
  std::vector<PartialNode*> back_;  // back pointers
  std::size_t in_ptrs_size_;
  // G* graph_;
  vector<PartialNode*>* root_inicio;
  bool root;

  // PartialNode(data_type data, std::size_t const& out_ptrs_size,
  // ADE::Persistence::PartialDirectedGraph<data_type,PartialNode<Type>>* graph)
  PartialNode(data_type data, std::size_t const& out_ptrs_size)
      : Node<Type>(data, out_ptrs_size),
        // graph_(graph),
        usedMods(0),
        root(false),
        in_ptrs_size_(0) {}

  PartialNode* look_root(unsigned int version) {    //me retorna el actual root
    bool es_raiz = false;
    PartialNode *puntero_elegido, *temp;
    bool last_root = true;
    if (this->root) {
      for (auto r : *root_inicio) {
        if (r->version_ > version) {
          last_root = false;
          puntero_elegido = temp;
          break;
        }
        temp = r;
      }
      if (last_root) {
        puntero_elegido = root_inicio->at(root_inicio->size() - 1);
      }
      return puntero_elegido;
    }
    else {
      return nullptr;
    }
  }
  

  
  data_type get_data() { return this->data_; }
  data_type get_data(unsigned int version) {
    data_type dato;
    bool data_change = false;
    //if
    bool es_raiz = false;
    PartialNode* root_elegido = look_root(version);
    if (root_elegido) {
      
      if (root_elegido->version_ == version) {
        return *(root_elegido->data_);
      }
      for (auto e : root_elegido->mods) {
        if (e->version <= version) {
          if (!e->puntero) {  // si es que no es un puntero cambiado sino un
                              // dato
            dato = e->value;
            data_change = true; //significa que el dato cambió, de lo contrario el dato no tuvo mod hasta esa versión
          }
        } else {
          break;
        }
      }
      if (!data_change) {
        return *(this->data_);
      }
      return dato;
    } 
    if (this->version_ > version) {
      cout << "This node doesn't exist yet on that version"<<endl;
      return -1;
    }
    for (auto e : mods) {
      if (e->version <= version) {
        if (!e->puntero) {  // si es que no es un puntero cambiado sino un dato
          dato = e->value;
          data_change = true;
        }
      } else {
        break;
      }
    }
    if (!data_change) {
      return *(this->data_);
    }
    return dato;
    // return *Node<Type>::data_;
  }
  
  PartialNode* verify_mods(unsigned int version) {//esta función va a ser recursiva en caso necesite hacer un split y los nodos
                                //a los que tenga atrás también deban hacer split y así sucesivamente
                                
                                
      
                            
    if (mods.size() >= 2 * in_ptrs_size_) { // en caso pase 2p hace un split                              
      PartialNode<Type>* new_nodo =
          new PartialNode<Type>(*(this->data_), this->out_ptrs_size_);

      // new_nodo->forward_ = this->forward_;
      new_nodo->back_ = this->back_;
      new_nodo->in_ptrs_size_ = in_ptrs_size_;
      new_nodo->version_ = version;
      new_nodo->root_inicio = this->root_inicio;
      

      for (auto m : mods) {
        // for (auto m = mods.begin(); m != mods.end(); m++) {
        if (m->puntero == false) {  // modifica el dato
          *(new_nodo->data_) = m->value;
        } else {
          new_nodo->forward_[m->index] =
              m->ptr;  // modifica y actualiza sus fordward
        }
      }
      // el nuevo nodo a este punto tiene sus datos correctos
      for (int i = 0; i < this->out_ptrs_size_; i++) {
        // for (auto f : new_nodo->forward_) {
        PartialNode* p = (dynamic_cast<PartialNode*>(forward_[i]));
        if (p)  // si existe
          for (int j = 0; j < p->back_.size(); j++) {
            // for (auto e : (dynamic_cast<PartialNode*>(*forward_[i]))->back_)
            // {
            if (p->back_[j] == this) {
              // e = new_nodo;  // actualizamos backpointer de los nodos forward
              // de
              // este nodo
              p->back_[j] = new_nodo;
            }
          }
      }
      int k = 0;
      for (auto b : new_nodo->back_) {
        k = 0;
        // for (auto f : (*b)->forward_) {
        if (b) {
          /*for (int i = 0; i < b->out_ptrs_size_; i++) {
              PartialNode* coin=dynamic_cast<ADE::Persistence::PartialNode<data_type>*> (b->forward_[i]);
            
            ADE::Node<data_type>* forw = b->forward_[i];
            if (b->forward_[i]) {
                cout << "dato " << b->forward_[i] << endl;
            }*/
            vector<PartialNode*> posActualizadas(this->out_ptrs_size_);
            for (auto bMod : b->mods) {
                if (bMod->puntero ) {
                    if (bMod->ptr == this) {
                        posActualizadas[bMod->index]=this;
                    } else {
                      posActualizadas[bMod->index] = nullptr;
                    }
                }
            }
            for (int i = 0; i < out_ptrs_size_; i++) {
              if (posActualizadas[i]== this) {
              // b->forward_[i] = new_nodo;      //actualizamos los forward de
              // los que nos apuntan    f -- (*b)->forward_[i]
                  PartialNode* pointer = b->verify_mods(version);
                  if (!pointer) {  // si hay espacio en modificaciones de esos
                                        // nodos que me apuntan sino será recursivo
                                        // y creando nuevos nodos
                    b->mods.push_back(
                        new modi<PartialNode, data_type>(i, new_nodo, version));
                    usedMods++;
                    // Colocamos mods en los nodos que apuntan a este para que
                    // apunten al nuevo
                  } else {
                    pointer->forward_[i] =dynamic_cast<ADE::Node<data_type>*> (new_nodo);
                  }
              }
               k++;
            }
        }
      }

      return new_nodo;
    } else {
      return nullptr;  // Hay espacio een mods
    }
  }

 

  bool set_data(data_type const data, unsigned int version) {
    
    PartialNode* ptr,*root_elegido=look_root(version);

    if (root_elegido) {
      if (!(ptr =root_elegido->verify_mods(version)))  // en caso sea nulo, es decir que tenga
                                        // espacio para mods inserta aquí
        root_elegido->mods.push_back(new modi<PartialNode, data_type>(data, version));
      else {
        
         ptr->root = true;
        ptr->version_ = version;
        *(ptr->data_) = data;  // simplemente le modifica el valor
                                           // porque es un split
        root_inicio->push_back(ptr);
      }
      ++usedMods;
      //*Node<Type>::data_ = data;
      return true;
    }

    if (!(ptr = verify_mods(version)))  // en caso sea nulo, es decir que tenga espacio
                               // para mods inserta aquí
      this->mods.push_back(new modi<PartialNode,data_type>(data, version));
    else {
      if (this->root == true) {
        ptr->root = true;
      }
      *(ptr->data_)=data; //simplemente le modifica el valor porque es uno nuevo
      //root_inicio->push_back(ptr);
    }
    ++usedMods;
    //*Node<Type>::data_ = data;
    return true;
  }

  bool set_ptr(PartialNode* ptr, unsigned int id) {
    Node<Type>::forward_ = ptr;
    return true;
  }
  PartialNode& operator[](std::size_t id) const {
    return *dynamic_cast<PartialNode*>(&(Node<Type>::operator[](id)));
  }
  PartialNode& operator[](
      std::pair<std::size_t, unsigned int> id_version) const {
    PartialNode* ptr;
    if (!this) {
      cout << "You are trying to access a null value" << endl;
      //return;
    }
    try {
      ptr =
          dynamic_cast<PartialNode*>(this->forward_[id_version.first]);
    } catch(std::exception& e) {
      cout << e.what() << endl;
      //return nullptr;
    }
    bool es_raiz = false;
    PartialNode* puntero_elegido,*temp;
    bool last_root = true;
    if (this->root) {
      for (auto r : *root_inicio) {
        if (r->version_ > id_version.second) {
          last_root = false;
          puntero_elegido = temp;
          break;
        }
        temp = r;
      }
      if (last_root) {
        puntero_elegido = root_inicio->at(root_inicio->size() - 1);
      }
      bool mod_change = false;
      for (auto modificacion : puntero_elegido->mods) {
        if (modificacion->version > id_version.second) break;
        if (modificacion->puntero) {
          if (modificacion->index == id_version.first) {
            ptr = modificacion->ptr;
            mod_change = true;
          }
        }
      }
      
      if (mod_change) {
        return *ptr;
      } else {
        ptr =dynamic_cast<PartialNode*>(puntero_elegido->forward_[id_version.first]);
        return *(ptr);
      }
    }
        
      
    
    bool mod_change = false;
    for (auto modificacion : mods) {
      if (modificacion->version > id_version.second)
          break;
      if (modificacion->puntero) {
          if (modificacion->index == id_version.first) {
            ptr = modificacion->ptr;
            mod_change = true;
          }
      } 
    }
    if (mod_change) {
      return *ptr;
    } else {
      this->forward_[id_version.first];
    }
    return *ptr;
    //return *dynamic_cast<PartialNode*>(&(Node<Type>::operator[](id_version.first)));
  }
};

template <typename Type, typename Node = PartialNode<Type>>
class PartialDirectedGraph : public DirectedGraph<Type, Node> {
 public:
  typedef Type data_type;
  typedef PartialDirectedGraph<Type> self;

  PartialDirectedGraph(data_type const data, std::size_t const& out_ptrs_size,
                       std::size_t const& in_ptrs_size)
      : DirectedGraph<Type, Node>(data, out_ptrs_size),
        in_ptrs_size_(in_ptrs_size),
        current_version(0) {
    Node* p = this->root_ptr_;
    p->in_ptrs_size_ = this->in_ptrs_size_;
    p->version_ = 0;
    roots.push_back(p);
    p->root_inicio = &(this->roots);
    p->root = true;
  }
  /*Node* look_root(unsigned int version) {  // me retorna el actual root
    bool es_raiz = false;
    PartialNode *puntero_elegido, *temp;
    bool last_root = true;
    if (this->root) {
      for (auto r : *root_inicio) {
        if (r->version_ > version) {
          last_root = false;
          puntero_elegido = temp;
          break;
        }
        temp = r;
      }
      if (last_root) {
        puntero_elegido = root_inicio->at(root_inicio->size() - 1);
      }
      return puntero_elegido;
    } else {
      return nullptr;
    }
  }*/
  Node* get_root_ptr(unsigned int version) {
    //return this->root_ptr_;
      
    bool es_raiz = false;
    Node *puntero_elegido, *temp;
    bool last_root = true;
    
      for (auto r : roots) {
        if (r->version_ > version) {
          last_root = false;
          puntero_elegido = temp;
          break;
        }
        temp = r;
      }
      if (last_root) {
        puntero_elegido = roots[roots.size() - 1];
      }
      return puntero_elegido;
    
  }

  Node* insert_vertex(data_type const data, Node* u, std::size_t position) {
    ++current_version;
    Node* p = insert_vertex(data, u, position, current_version);    
    p->in_ptrs_size_ = this->in_ptrs_size_;     //coloca al vertice ingresado in_ptr
    p->version_ = current_version;
    p->root_inicio = &(this->roots);
    return p;
  }
  bool set_data(Node* u, data_type data) {
    ++current_version;
    u->set_data(data, current_version);
    return true;
  }

  void add_edge(Node* u, Node* v, std::size_t position) {
        
    try {
      if (v->in_ptrs_size_ <= v->back_.size()) {
        cout << "You have exceeded the in_ptrs_size " << endl;
        return;  // no hace nada
      }
      if (u->out_ptrs_size_ <= position) {
        cout << "You are out of range" << endl;
        return;  // no hace nada
      }
    } catch (exception& e) {
      cout << e.what() << endl;
    }

    ++current_version;
    return add_edge(u, v, position, current_version);
  }

 private:
  Node* insert_vertex(data_type const data, Node* u, std::size_t position,
                      unsigned int version) {
    Node* next;
    if(u)
        next= dynamic_cast<Node*>(u->forward_[position]);

    /*Node* p = dynamic_cast<Node*>(
        DirectedGraph<Type, Node>::insert_vertex(data, u, position));*/
    
    ADE::Node<Type>* next_node_ptr = u->forward_[position];
    Node* new_node = new Node(data, out_ptrs_size_);
    new_node->forward_[position] = next_node_ptr;
    //u->forward_[position] = new_node;
    //return dynamic_cast<Node*>(u->forward_[position]);
    Node* ptrSplit;
    if (!(ptrSplit = u->verify_mods(version))) {
      u->mods.push_back(new modi<Node, data_type>(position, new_node, version));
    } else {
      ptrSplit->forward_[position] =dynamic_cast<ADE::Node<data_type>*>(new_node);
      
    }
    //ADE::Persistence::PartialNode<data_type>* new_nodoP = dynamic_cast<ADE::Persistence::PartialNode<data_type>*>(new_node);
    if (u && new_node)
       new_node->back_.push_back(u);
    if (next) {
      next->back_.push_back(new_node);
      int j = 0;
      //for (auto n = next->back_.begin(); n < next->back_.end(); j++,n++) {

      for (auto n : next->back_) {          //buscamos el u[pos]=next para borrarle su backpointer a u
        if (n!=nullptr) {
          if (n == u) {
            next->back_.erase(next->back_.begin()+j);
          }
        }
        j++;
      }
    }
    new_node=new_node;
    return new_node;
  }

  void add_edge(Node* u, Node* v, std::size_t position, unsigned int version) {
    //u->forward_[position] = v;  // agregamos el edge
    Node* ptr;
    u->usedMods++;
    if (!(ptr=u->verify_mods(version))) {

      modi<Node, data_type>* nuevo =new modi<Node, data_type>(position, v, version);
      u->mods.push_back(nuevo);  // agregamos la mod a u
                                                          
    }
    else {
      if (u == this->root_ptr_) {
        roots.push_back(ptr);
        ptr->root = true;
        //this->root_ptr_ = ptr;
      }
      ptr->forward_[position] = v;  // agregamos el edge en el nuevo nodo creado
    }
    if (v) {
      v->back_.push_back(u);    //agregamos a v el backpointer  u
    }
    return;
    //return DirectedGraph<Type, Node>::add_edge(u, v, position);
  }

  std::size_t in_ptrs_size_;
  unsigned int current_version;
  std::vector<Node*> roots;
};

}  // namespace Persistence
}  // namespace ADE

#endif  // SOURCE_PARTIALDIRECTEDGRAPH_HPP_