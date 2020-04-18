// Copyright 2020 Roger Peralta Aranibar
#include <cassert>
#include <exception>
#include <iostream>
#include <memory>
#include <vector>

#include "DirectedGraph.hpp"
#include "PartialDirectedGraph.hpp"

int main() {
  // Create a DirectedGraph, a maximum of 5 edged each node and the root with
  // the starting valuee of 1.
  ADE::Persistence::PartialDirectedGraph<int> my_graph(1, 5, 3);

  // We can obtain the root and print their value.
  ADE::Persistence::PartialNode<int>* root_ptr = my_graph.get_root_ptr(1);
  std::cout << "Root Value: " << *root_ptr->data_ << std::endl;

  // Also, we can insert a new vertex passing the new Node value, a Node
  // reference and the index of the pointer used from the reference node to
  // the
  // inserted node.
  ADE::Persistence::PartialNode<int>* inserted_node_ptr =
      my_graph.insert_vertex(2, root_ptr, 0);

  // Likewise, the method to insert a new vertex, returns a reference of the
  // new
  // vertex inserted
  ADE::Persistence::PartialNode<int>* other_new_node_ptr =
      my_graph.insert_vertex(3, inserted_node_ptr, 1);

  // To add an edge between two vertex, we call the function using both Nodes
  // references and the index pointer that will link the first node to the
  // second.
  my_graph.add_edge(other_new_node_ptr, root_ptr, 3);

  // We traverse the Graph from a reference node (using any Node pointer
  // provided by the previous methods), with the operator []
  ADE::Persistence::PartialNode<int> retrived_node =
      (*root_ptr)[std::pair<std::size_t, unsigned int>(0, 4)]
                 [std::pair<std::size_t, unsigned int>(1, 4)];
  std::cout << "Node Value: " << *retrived_node.data_ << std::endl;

  // It is also possible to insertt between two Nodes.
  //ADE::Persistence::PartialNode<int>* between_node_ptr =my_graph.insert_vertex(5, inserted_node_ptr, 1);
  retrived_node = (*root_ptr)[std::pair<std::size_t, unsigned int>(0, 5)]
                             [std::pair<std::size_t, unsigned int>(1, 5)];
  std::cout << "Between Value: " << *retrived_node.data_ << std::endl;

  retrived_node = (*root_ptr)[std::pair<std::size_t, unsigned int>(0, 5)]
                             [std::pair<std::size_t, unsigned int>(1, 5)]
                             [std::pair<std::size_t, unsigned int>(3, 5)];
  std::cout << "Next Value: " << *retrived_node.data_ << std::endl;



  //PROBANDO SPLIT
  ADE::Persistence::PartialNode<int>* another_new_node_ptr =
      my_graph.insert_vertex(4, inserted_node_ptr, 1);
   //ver 4

  //ver5
  my_graph.add_edge(root_ptr,other_new_node_ptr, 1);        //5

  //ver6
  my_graph.add_edge(root_ptr, inserted_node_ptr, 2);        //6
  //ver7
  my_graph.add_edge(root_ptr, another_new_node_ptr, 3); //7

  my_graph.add_edge(another_new_node_ptr, inserted_node_ptr, 1);  // 8
  
  my_graph.set_data(inserted_node_ptr, 10);     //9 version
  my_graph.set_data(inserted_node_ptr, 20);     //10 version
  my_graph.set_data(inserted_node_ptr, 30);     //11 version
  my_graph.set_data(inserted_node_ptr, 40);
  my_graph.set_data(inserted_node_ptr, 50);     //13 version    //hace split y el nuevo nodo tiene 50
  

  cout << "Inserted_ptr split 1: " << inserted_node_ptr->get_data(9) << endl;
  cout << "Inserted_ptr split 2: " << inserted_node_ptr->get_data(11) << endl;

  retrived_node = (*root_ptr)[std::pair<std::size_t, unsigned int>(0, 10)];
  std::cout << "Retrieved 1 Value: " << retrived_node.get_data(10) << std::endl;
  

  my_graph.set_data(root_ptr, 100);     //14 version
  my_graph.set_data(root_ptr, 200);
  my_graph.set_data(root_ptr, 300);     //16
  my_graph.set_data(root_ptr, 400);     //17 version

  retrived_node = (*root_ptr)[std::pair<std::size_t, unsigned int>(0, 20)]
                             [std::pair<std::size_t, unsigned int>(1, 20)];
  std::cout << "Retrieved 2 Value: " << retrived_node.get_data(20) << std::endl;
  root_ptr = my_graph.get_root_ptr(15);
  cout << "Root split 1: " << root_ptr->get_data(15) << endl;
  cout << "Root split 2: " << root_ptr->get_data(16) << endl;

 

  retrived_node = (*root_ptr)[std::pair<std::size_t, unsigned int>(0, 20)]
                             [std::pair<std::size_t, unsigned int>(1, 20)]
                             [std::pair<std::size_t, unsigned int>(1, 20)];
  std::cout << "Retrieved 2 Value: " << retrived_node.get_data(20) << std::endl;
  //root_ptr = my_graph.get_root_ptr(20);
  cout<<"ROOT 0 version " <<root_ptr->get_data(0)<<endl;
  cout << "ROOT 0 version " << root_ptr->get_data(14) << endl;

  return 0;
}