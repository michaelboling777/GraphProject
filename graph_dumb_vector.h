#ifndef _GRAPH_VECTOR_H_
#define _GRAPH_VECTOR_H_

#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>


template<typename VertexProperty, typename EdgeProperty>
  class graph_vector {

    //you have to forward declare these so you can use them in the public
    //section, when they are defined in the private section below. Essentially,
    //the compiler needs to know they exist in order to use them before the
    //definition of them.
    class vertex;
    class edge;

    public:
      ///required public types
      typedef size_t vertex_descriptor; //unique vertex identifier

      typedef std::pair<size_t, size_t> edge_descriptor; //unique edge identifier
                                                    //represents pair of vertex
                                                    //descriptors

      typedef std::vector<vertex*> vertex_storage;
      typedef std::vector<edge*> edge_storage;
      typedef std::vector<edge*> adj_edge_storage;
      //vertex container should contain "vertex*" or shared_ptr<vertex>
      typedef typename vertex_storage::iterator vertex_iterator; //vertex iterators
      typedef typename vertex_storage::const_iterator const_vertex_iterator;

      //edge container should contain "edge*" or shared_ptr<edge>
      typedef typename edge_storage::iterator edge_iterator; //edge iterators
      typedef typename edge_storage::const_iterator const_edge_iterator;

      //adjacent edge container should contain "edge*" or shared_ptr<edge>
      typedef typename adj_edge_storage::iterator adj_edge_iterator; //adjacency list iterators
      typedef typename adj_edge_storage::const_iterator const_adj_edge_iterator;

      ///required constructor/destructors
      graph_vector() : m_max_vd(0) { }

      ~graph_vector() {
        clear();
      }

      graph_vector(const graph_vector&) = delete;
      graph_vector& operator=(const graph_vector&) = delete;

      ///required graph operations

      //iterators
      vertex_iterator vertices_begin() {return m_vertices.begin();}
      const_vertex_iterator vertices_cbegin() const {return m_vertices.cbegin();}
      vertex_iterator vertices_end() {return m_vertices.end();}
      const_vertex_iterator vertices_cend() const {return m_vertices.cend();}

      edge_iterator edges_begin() {return m_edges.begin();}
      const_edge_iterator edges_cbegin() const {return m_edges.cbegin();}
      edge_iterator edges_end() {return m_edges.end();}
      const_edge_iterator edges_cend() const {return m_edges.cend();}

      //accessors
      size_t num_vertices() const {return m_vertices.size();}
      size_t num_edges() const {return m_edges.size();}

      vertex_iterator find_vertex(vertex_descriptor vd) {
        return std::find_if(m_vertices.begin(), m_vertices.end(),
            [&](const vertex* const v) {
            return v->descriptor() == vd;
            });
      }

      const_vertex_iterator find_vertex(vertex_descriptor vd) const {
        return std::find_if(m_vertices.cbegin(), m_vertices.cend(),
            [&](const vertex* const v) {
            return v->descriptor() == vd;
            });
      }

      edge_iterator find_edge(edge_descriptor ed) {
        return std::find_if(m_edges.begin(), m_edges.end(),
            [&](const edge* const e) {
            return e->descriptor() == ed;
            });
      }

      const_edge_iterator find_edge(edge_descriptor ed) const {
        return std::find_if(m_edges.cbegin(), m_edges.cend(),
            [&](const edge* e) {
            return e->descriptor() == ed;
            });
      }

      //@todo modifiers
      vertex_descriptor insert_vertex(const VertexProperty& vp) {

        //construct vertex obj and assign it to var of appr type
      auto ins_vert = new vertex(m_max_vd, vp);
    
      //insert that into the vector that contains all vertices 
      m_vertices.push_back(ins_vert);
    
      //add the incrementation to the return value to insure the next insertion gets its unique 
      //vertex descriptor...
		return m_max_vd++;
      }

      edge_descriptor insert_edge(vertex_descriptor sd, vertex_descriptor td,
          const EdgeProperty& ep) {

            //create edge obj with given passed parameters and assign to var of appropriate type
      auto ins_edg = new edge(sd, td, ep);

      //insert the given obj into the vector that contains all edges
      m_edges.push_back(ins_edg);
    
      //use find_vertex() so find vertex with given descriptor and assign it to
      //var of appropriate type. Important since this represents the vertex that is one side of the edge 
      //being inserted
      auto src_v = find_vertex(sd);

      //Now, since we have source vertex we dereference the vertex of interest 
      //and insert the given edge into that particular vertex's outgoing edge container. 
      //In this case, its an outgoing edge since the vertex of interest is assummed to be a source vertex.
      (*src_v)->m_out_edges.push_back(ins_edg);
    
      //Now, we essentially do the same thing but for the target destination vertex
      //since, inserting an edge is going to be connected with 2 vertices 

      //use find_vertex() so find vertex with given descriptor and assign it to
      //var of appropriate type. Important since this represents the vertex that is the other side of the edge 
      //being inserted 
      auto targ_v = find_vertex(td);

      //Now, since we have target vertex we dereference the vertex of interest 
      //and insert the given edge into that particular vertex's incoming edge container. 
      //In this case, its an incoming edge since the vertex of interest is assummed to be a target vertex.
      (*targ_v)->m_in_edges.push_back(ins_edg);


		return {sd,td};
      }

      void insert_edge_undirected(vertex_descriptor sd, vertex_descriptor td,
          const EdgeProperty& ep) {

             //call function to insert edge with sd and td in their appropriate places
      //essentially making a directed edge at this point
      insert_edge(sd, td, ep);
      //if we swap sd and td places then the graph additionally becomes a directed graph 
      //in the other direction as well 
      insert_edge(td, sd, ep);

      //so now one edge is directed from sd to td and the other edge is directed from td to sd
      //essentially making a undirected edge
        
      }

      void erase_vertex(vertex_descriptor vd) {

        //find_vertex() to find the vertex passed by function and assign it to a variable with a necessary type
    auto erase_v = find_vertex(vd);

    //we check if the passed vertex was accurately found from our previous line of code..
    if (erase_v != vertices_end()) {

  //loop over each outgoin edge in container associated with the 
       //vertex that was passed to the function and
       //assign it var of appropriate type so we 
       //can perform necessary erase function on it. It will erase each of 
       // the vertex's 
       //associated outgoing edges and erase the outgoing edge from outgoing edge container
       //and erase incoming edge from incoming edge container
    for (auto erase_out_edg : (*erase_v)->m_out_edges) {
        erase_edge(erase_out_edg->descriptor());
    }

     
    delete *erase_v;  

    //erase vertex from vector that contains all the vertices....
    m_vertices.erase(erase_v);  
}
       
      }

      void erase_edge(edge_descriptor ed) {

         //find the edge that was passed to us and assign it to var of its necessary type
       auto erase_edg = find_edge(ed);

     //we check if the passed edge was accurately found from our previous line of code..
    if (erase_edg != edges_end()) {

      //we remember that every time we inserted an edge we created a pair between the source and target vertices
      //associated with that particular inserted edge as from insert_edge()... 
      //so now we can correctly identify the vertices 
      //among the edge we wish to erase from the graph. And we find them using the 
      //find_vertex() function to do so.
    auto src_v = find_vertex(ed.first);
    auto targ_v = find_vertex(ed.second);

    //if the vertices we just searched for were indeed found...
    if (src_v != vertices_end() && targ_v != vertices_end()) {

         //search for erase_edg in outgoing edge container (for source vertex) and incoming 
        //edge container (for the target vertex)
        //and they should exists since when we inserted an edge ... we also inserted
        //the edge in the source and target vertices outgoing and incoming edge container
        //respectively as well.
        auto erase_src_edg = find((*src_v)->m_out_edges.begin(), (*src_v)->m_out_edges.end(), *erase_edg);
        auto erase_targ_edg = find((*targ_v)->m_in_edges.begin(), (*targ_v)->m_in_edges.end(), *erase_edg);

        //therefore, now that we have a hold of the particular edge with the associated vertex with its
        //associated container... we can now easily erase those edges associated with 
        //those given vertices that are either from m_out_edges container or m_in_edges container
        //(dependent on if associated vertex was source or target)
        (*src_v)->m_out_edges.erase(erase_src_edg);
        (*targ_v)->m_in_edges.erase(erase_targ_edg);
    }


    delete *erase_edg; 

    //erase the edge from the vector that contains all edges 
    m_edges.erase(erase_edg);  
}


        
      }
      // end @todo
      void clear() {
        m_max_vd = 0;
        for(auto v : m_vertices)
          delete v;
        m_vertices.clear();
        for(auto e : m_edges)
          delete e;
        m_edges.clear();
      }

      template<typename V, typename E>
        friend std::istream& operator>>(std::istream& is, graph<V, E>& g);

      template<typename V, typename E>
        friend std::ostream& operator<<(std::ostream& os, const graph<V, E>& g);

    private:

      size_t m_max_vd; // Id generator for next vertex to be inserted
      vertex_storage m_vertices;  // List of all vertices in the graph
      edge_storage m_edges;    // List of  all edges in the graph

      ///required internal classes

      class vertex {
        public:
          ///required constructors/destructors
          vertex(vertex_descriptor vd, const VertexProperty& v) :
            m_descriptor(vd), m_property(v) { }

          ///required vertex operations

          //iterators
          adj_edge_iterator begin() {return m_out_edges.begin();}
          const_adj_edge_iterator cbegin() const {return m_out_edges.cbegin();}
          adj_edge_iterator end() {return m_out_edges.end();}
          const_adj_edge_iterator cend() const {return m_out_edges.cend();}

          //accessors
          const vertex_descriptor descriptor() const {return m_descriptor;}
          VertexProperty& property() {return m_property;}
          const VertexProperty& property() const {return m_property;}

        private:
          vertex_descriptor m_descriptor; // Unique id assigned during insertion
          VertexProperty m_property;      // Label or weight passed during insertion
          adj_edge_storage m_out_edges;   // Outgoing edges
		  adj_edge_storage m_in_edges;   // Outgoing edges
  
          friend class graph_vector;
      };

      class edge {
        public:
          ///required constructors/destructors
          edge(vertex_descriptor s, vertex_descriptor t,
              const EdgeProperty& w) : m_source(s), m_target(t), m_property(w) { }

          ///required edge operations

          //accessors
          const vertex_descriptor source() const {return m_source;}
          const vertex_descriptor target() const {return m_target;}
          const edge_descriptor descriptor() const {return {m_source, m_target};}
          EdgeProperty& property() {return m_property;}
          const EdgeProperty& property() const {return m_property;}

        private:
          vertex_descriptor m_source; // Descriptor of source vertex
          vertex_descriptor m_target;  // Descriptor of target vertex
          EdgeProperty m_property;    // Label or weight on the edge
      };

  };

template<typename V, typename E>
  std::istream& operator>>(std::istream& is, graph_vector<V, E>& g) {
    size_t num_verts, num_edges;
    is >> num_verts >> num_edges;
    g.m_vertices.reserve(num_verts);
    g.m_vertices.reserve(num_edges);
    for(size_t i = 0; i < num_verts; ++i) {
      V v;
      is >> v;
      g.insert_vertex(v);
    }
    for(size_t i = 0; i < num_edges; ++i) {
      typename graph<V, E>::vertex_descriptor s, t;
      E e;
      is >> s >> t >> e;
      g.insert_edge(s, t, e);
    }
    return is;
  }

template<typename V, typename E>
  std::ostream& operator<<(std::ostream& os, const graph_vector<V, E>& g) {
    os << g.num_vertices() << " " << g.num_edges() << std::endl;
    for(auto i = g.vertices_cbegin(); i != g.vertices_cend(); ++i)
      os << (*i)->property() << std::endl;
    for(auto i = g.edges_cbegin(); i != g.edges_cend(); ++i)
      os << (*i)->source() << " " << (*i)->target() << " "
        << (*i)->property() << std::endl;
    return os;
  }



#endif
