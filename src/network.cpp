#include "network.h"

#include "random.h"
#include <algorithm>
#include <stdexcept>
#include <string>

//used to sort in decreasing order
bool reverse_comparator(const double& a, const double& b)
{
	return (a > b);
}

void Network::resize(const size_t &n)
{
	//clear existing values
	values.clear();
	
	//generate new values
	for(size_t i(0); i < n; i++)
	{
		values.push_back(RNG.normal(0.0, 1.0));
	}
	
	//update number of nodes
	number_of_nodes = n;
}

bool Network::add_link(const size_t& a, const size_t& b)
{
	if(a < size() && b < size())
	{
		//connection between the same node is impossible (does not throw an exception)
		if(a == b)
		{
			return false;
		}
		
		//verifying link non-existence (only verify one direction, assuming the addition of link is solely controled by this function)
		std::vector<size_t> neighbor_a = neighbors(a);
		
		if(std::find(neighbor_a.begin(), neighbor_a.end(), b) == neighbor_a.end())
		{
			//add bidirectional link
			links.insert(std::make_pair(a, b));
			links.insert(std::make_pair(b, a));
			
			return true;
		}
		return false;
	}
	else return false; //(does not throw an exception) 
}

size_t Network::random_connect(const double& mean_deg)
{
	//clear existing links
	links.clear();
	
	//generate new links
	size_t total_links(0);
	
	int number_of_links;
	int degree_node;
	
	//go through each node
	for(size_t node(0); node < size(); node++)
	{
		degree_node = degree(node);
		//generate additional (poisson(mean_deg)) links to existing ones (for a total of degree_node+poisson(mean_deg)), can not generate more links in totals than you have nodes
		number_of_links = std::min(RNG.poisson(mean_deg), int(size()) - degree_node);
		
		//generate only if number of links you wish to have is superior to the number of existing links to this node
		if(number_of_links > 0)
		{
			//generate number_of_links links to random (uniform)
			std::vector<size_t> possible_neighbor = possible_neighbors(node);
			int possible_neighbor_original_size = possible_neighbor.size();
			
			for(size_t links_created(0); links_created < number_of_links; links_created++)
			{
				//select a node from the possible neighbor (via possible neighbor index : 0 through (possible_neighbor_original_size-1)-links_created, the possible neighbor is reduced by 1 neighbor at each node chosen)
				int selected_possible_neighbor_index = RNG.uniform_int(0, ((possible_neighbor_original_size-1)-links_created));
				
				//add link
				add_link(node, possible_neighbor[selected_possible_neighbor_index]);
				
				//remove possible neighbor
				std::swap(possible_neighbor[selected_possible_neighbor_index], possible_neighbor.back());
				possible_neighbor.pop_back();
			}
			
			total_links += number_of_links;
		}
	}
	
	return total_links;
}

size_t Network::set_values(const std::vector<double>& new_values)
{
	size_t new_values_size(std::min(new_values.size(), size()));
	
	for(size_t node(0); node < new_values_size; node++)
	{
		values[node] = new_values[node];
	}
	
	return new_values_size;
}

size_t Network::size() const
{
	return number_of_nodes;
}

size_t Network::degree(const size_t &_n) const
{
	if(_n < size())
	{
		return links.count(_n);
	}
	else
	{
		std::string message("");
		message += "the node " + std::to_string(_n) + " you try to access does not exist.";
		throw std::out_of_range(message);
	}
}

double Network::value(const size_t &_n) const
{
	if(_n < size())
	{
		return values[_n];
	}
	else
	{
		std::string message("");
		message += "the node " + std::to_string(_n) + " you try to access does not exist.";
		throw std::out_of_range(message);
	}
}

std::vector<double> Network::sorted_values() const
{
	//copy of values
	std::vector<double> sorted_value = values;
	
	//sorting
	std::sort(sorted_value.begin(), sorted_value.end(), reverse_comparator);
	
	return sorted_value; 
}

std::vector<size_t> Network::neighbors(const size_t& n) const
{
	if(n < size())
	{
		std::vector<size_t> neighbor;
		
		//go through each links binding node n to another one
		auto iterator_start = links.lower_bound(n);
		auto iterator_end = links.upper_bound(n);
		
		
		for(auto iterator = iterator_start; iterator != iterator_end; iterator++)
		{
			//add the node to which node n is linked
			neighbor.push_back(iterator->second);
		}
		
		return neighbor;
	}
	else
	{
		std::string message("");
		message += "the node " + std::to_string(n) + " you try to access does not exist.";
		throw std::out_of_range(message);
	}
}

std::vector<size_t> Network::possible_neighbors(const size_t& n) const
{
	if(n < size())
	{
		std::vector<size_t> neighbor = neighbors(n);
		
		std::vector<size_t> possible_neighbor;
		
		//generate list of possible_neighbor by cycling from 0 to size()-1 and adding the node which are not linked to n
		for(size_t node(0); node < size(); node++)
		{
			//no link to the same node
			if(node != n)
			{
				auto iterator = std::find(neighbor.begin(), neighbor.end(), node);
				
				//not linked to n
				if(iterator == neighbor.end()) possible_neighbor.push_back(node);
				else
				{
					//removing the node from the neighbor list because we have already checked it and wish to refrain doing so further down the loop.
					neighbor.erase(iterator);
				}
			}
		}
		
		return possible_neighbor;
	}
	else
	{
		std::string message("");
		message += "the node " + std::to_string(n) + " you try to access does not exist.";
		throw std::out_of_range(message);
	}
}
