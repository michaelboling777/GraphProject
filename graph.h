#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <iostream>
#include <list>
#include <utility>
#include <algorithm>
#include <memory>
#include <unordered_set>
#include <set>
// Uncomment if using unordered_set to store edges with boost hash function
//#include <boost/functional/hash.hpp>



////////////////////////////////////////////////////////////////////////////////
/// A generic adjacency-list graph where each vertex stores a VertexProperty and
/// each edge stores an EdgeProperty.
////////////////////////////////////////////////////////////////////////////////
template<typename VertexProperty, typename EdgeProperty>
class graph {

  // The vertex and edge classes are forward-declared to allow their use in the
  // public section below. Their definitions follow in the private section
  // afterward.
  class vertex;
  class edge;
  
  struct vertex_hash;
  struct edge_hash;
  struct vertex_eq;
  struct edge_eq;
  struct edge_comp;

  public:

    // Required public types

    /// Unique vertex identifier
    typedef size_t vertex_descriptor;

    /// Unique edge identifier represents pair of vertex descriptors
    typedef std::pair<size_t, size_t> edge_descriptor;

    ///@brief A container for the vertices. It should contain "vertex*" or
    ///      shared_ptr<vertex>.
    typedef std::unordered_set<vertex*, vertex_hash, vertex_eq> MyVertexContainer;

    ///@brief A container for the edges. It should contain "edge*" or
    ///      shared_ptr<edge>.
    //typedef std::unordered_set<edge*, edge_hash, edge_eq> MyEdgeContainer;
	typedef std::set<edge*, edge_comp> MyEdgeContainer;

    ///@brief A container for the adjacency lists. It should contain
    ///      "edge*" or shared_ptr<edge>.
    //typedef std::unordered_set<edge*, edge_hash, edge_eq> MyAdjEdgeContainer;
	typedef std::set<edge*, edge_comp> MyAdjEdgeContainer;
	
	// Vertex iterators
    typedef typename MyVertexContainer::iterator vertex_iterator;
    typedef typename MyVertexContainer::const_iterator const_vertex_iterator;

    // Edge iterators
    typedef typename MyEdgeContainer::iterator edge_iterator;
    typedef typename MyEdgeContainer::const_iterator const_edge_iterator;

    // Adjacency list iterators
    typedef typename MyAdjEdgeContainer::iterator adj_edge_iterator;
    typedef typename MyAdjEdgeContainer::const_iterator const_adj_edge_iterator;

    // Required graph operations

    ///@brief Constructor/destructor
    graph() : m_max_vd(0) { }

    ~graph() {
        clear();
    }

    graph(const graph&) = delete;             ///< Copy is disabled.
    graph& operator=(const graph&) = delete;  ///< Copy is disabled.

    ///@brief vertex iterator operations
    vertex_iterator vertices_begin() {return m_vertices.begin();}
    const_vertex_iterator vertices_cbegin() const {return m_vertices.cbegin();}
    vertex_iterator vertices_end() {return m_vertices.end();}
    const_vertex_iterator vertices_cend() const {return m_vertices.cend();}
	  
    ///@brief  edge iterator operations
    edge_iterator edges_begin() {return m_edges.begin();}
    const_edge_iterator edges_cbegin() const {return m_edges.cbegin();}
    edge_iterator edges_end() {return m_edges.end();}
    const_edge_iterator edges_cend() const {return m_edges.cend();}

    ///@brief Define accessors
    size_t num_vertices() const {return m_vertices.size();}
    size_t num_edges() const {return m_edges.size();}

    vertex_iterator find_vertex(vertex_descriptor vd) {
      vertex v(vd, VertexProperty());
      return m_vertices.find(&v);
    }

    const_vertex_iterator find_vertex(vertex_descriptor vd) const {
      vertex v(vd, VertexProperty());
      return m_vertices.find(&v);
    }

    edge_iterator find_edge(edge_descriptor ed) {
      edge e(ed.first, ed.second, EdgeProperty());
      return m_edges.find(&e);
    }

    const_edge_iterator find_edge(edge_descriptor ed) const {
      edge e(ed.first, ed.second, EdgeProperty());
      return m_edges.find(&e);
    }

    ///@todo Define modifiers
    vertex_descriptor insert_vertex(const VertexProperty& vp){ 

      //construct vertex obj and assign it to appropriate var type
	  auto ins_vert = new vertex(m_max_vd, vp);
    //insert that into the unordered set that contains all vertices 
    m_vertices.insert(ins_vert);
    //add the incrementation to the return value to insure the next insertion gets its unique 
    //vertex descriptor...
	  return m_max_vd++;
	}
    edge_descriptor insert_edge(vertex_descriptor sd, vertex_descriptor td,
        const EdgeProperty& ep){

          //create edge obj with given passed parameters and assign to appropriate var
    auto ins_edg = new edge(sd, td, ep);
    
    //insert the given obj into the set that contains all edges 
    m_edges.insert(ins_edg);
    
    //use find_vertex() so find vertex with given descriptor and assign it to
    //appropriate var. Important since this represents the vertex that is one side of the edge 
    //being inserted  
    auto src_v = find_vertex(sd);
    
    //Now, since we have source vertex we dereference the vertex of interest 
    //and insert the given edge into that particular vertex's outgoing edge container. 
    //In this case, its an outgoing edge since the vertex of interest is assummed to be a source vertex.
    (*src_v)->m_out_edges.insert(ins_edg);
    
    //Now, we essentially do the same thing but for the target destination vertex
    //since, inserting an edge is going to be connected with 2 vertices 

    //use find_vertex() so find vertex with given descriptor and assign it to
    //appropriate var. Important since this represents the vertex that is the other side of the edge 
    //being inserted 
    auto targ_v = find_vertex(td);

    //Now, since we have target vertex we dereference the vertex of interest 
    //and insert the given edge into that particular vertex's incoming edge container. 
    //In this case, its an incoming edge since the vertex of interest is assummed to be a target vertex.
    (*targ_v)->m_in_edges.insert(ins_edg);
		return std::make_pair(sd, td);	
	}
    void insert_edge_undirected(vertex_descriptor sd, vertex_descriptor td,
        const EdgeProperty& ep){

           //call function to insert edge with sd and td in their appropriate places
          //essentially making a directed edge at this point
           insert_edge(sd, td, ep);
           //if we swap sd and td places then the graph additionally becomes a directed graph 
           //in the other direction as well
           insert_edge(td, sd, ep); 

           //so now one edge is directed from sd to td and the other edge is directed from td to sd
           //essentially making a undirected edge
			
	}
    void erase_vertex(vertex_descriptor vd){

      //use find_vertex() so find vertex with given descriptor and assign it to
      //appropriate var type.
        auto erase_v = find_vertex(vd);

        //checking if passed vertex was indeed found from our prev line...
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

      //erase vertex from unordered set that contains all the vertices....
        m_vertices.erase(erase_v);

        }
		
	}
    void erase_edge(edge_descriptor ed){

      //find the edge of interest that was passed that we wish to erase and assign to var of appropriate type 
      auto erase_edg = find_edge(ed);



      //if the edge we looked for that was passed to the function was found...
      if (erase_edg != edges_end()) {


        //we remember that every time we inserted an edge we created a pair between the source and target vertices
        //associated with that particular inserted edge as from insert_edge()... 
        //so now we can correctly identify the vertices 
        //among the edge we wish to erase from the graph. And we find them using the 
        //find_vertex() function to do so. 

        auto src_v = find_vertex(ed.first);
        auto targ_v = find_vertex(ed.second);

        //if the source and target vertices associated with the edge we wish to 
        //erase were indeed found ...
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

        //erase the edge from the set that contains all edges 
        m_edges.erase(erase_edg);
      }

	}
	////end of @todo
	
    void clear() {
      m_max_vd = 0;
      for(auto v : m_vertices)
        delete v;
      m_vertices.clear();
      for(auto e : m_edges)
        delete e;
      m_edges.clear();
    }

    // Friend declarations for input/output.
    template<typename V, typename E>
    friend std::istream& operator>>(std::istream&, graph<V, E>&);
    template<typename V, typename E>
    friend std::ostream& operator<<(std::ostream&, const graph<V, E>&);

  private:
	size_t m_max_vd; //< Maximum vertex descriptor assigned
    MyVertexContainer m_vertices; //<Contains all vertices
    MyEdgeContainer m_edges;    //<Contains all edges
    // Required internal classes

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

          vertex_descriptor m_descriptor; // Unique id for the vertex - assigned during insertion
          VertexProperty m_property;      // Label or property of the vertex - passed during insertion
          MyAdjEdgeContainer m_out_edges; // Container that includes the outgoing edges
		  MyAdjEdgeContainer m_in_edges; // Container that includes the incoming edges
          friend class graph;
    };


    ////////////////////////////////////////////////////////////////////////////
    /// Edges represent the connections between nodes in the graph.
    ////////////////////////////////////////////////////////////////////////////
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
        vertex_descriptor m_source; // Unique id of the source vertex
        vertex_descriptor m_target; // Unique id of the target vertex 
        EdgeProperty m_property;    // Label or weight of the edge 
    };
	
	struct vertex_hash {
        size_t operator()(vertex* const& v) const {
          return h(v->descriptor());
        }
        std::hash<vertex_descriptor> h;
    };
    // Uncomment if you have boost in your machine
   /* struct edge_hash {
	  // You can re-write this function to create the hash-value for a pair i.e., edge descriptor
	  // instead of using boost::hash
      size_t operator()(edge* const& e) const {
        return h(e->descriptor());
	  }
      boost::hash<edge_descriptor> h;
    };*/

    struct vertex_eq {
      bool operator()(vertex* const& u, vertex* const& v) const {
        return u->descriptor() == v->descriptor();
      }
    };

    struct edge_eq {
      bool operator()(edge* const& e, edge* const& f) const {
        return e->descriptor() == f->descriptor();
      }
    };
	
	struct edge_comp {
	  bool operator()(edge* const& e, edge* const& f) const {
        return e->descriptor() < f->descriptor();
      }
	};
};

///@brief Define io operations for the graph.
template<typename V, typename E>
std::istream& operator>>(std::istream& is, graph<V, E>& g) {
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
std::ostream& operator<<(std::ostream& os, const graph<V, E>& g) {
    os << g.num_vertices() << " " << g.num_edges() << std::endl;
    for(auto i = g.vertices_cbegin(); i != g.vertices_cend(); ++i)
      os << (*i)->property() << std::endl;
    for(auto i = g.edges_cbegin(); i != g.edges_cend(); ++i)
      os << (*i)->source() << " " << (*i)->target() << " "
        << (*i)->property() << std::endl;
    return os;
}


#endif
