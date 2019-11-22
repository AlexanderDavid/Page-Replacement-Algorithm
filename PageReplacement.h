#include <vector>
#include <iostream>
#include <cstdlib>
#include <deque>
#include<set>

/*!
 * \brief The AbstractPageReplacement class is an abstract definition
 * for the Page Replacement algorithm. All page replacement algorithms
 * must inherit from this class and override the page faults function
 *
 * \author Alex Day
 * \date November 12th 2019
 */
class AbstractPageReplacement 
{
public:

    /*!
     * \brief AbstractPageReplacement constructs ths Page Replacement abstract class
     * that takes all of the information required to calculate the numer of page faults.
     * Does not do any calculation and this specific class cannot calculate any page
     * faults. Nor can it actually be instantiated so this constructor is only for use
     * in the derived classes
     *
     * \param ref_string Ordered list of frame requests
     * \param num_pages Number of pages
     * \param num_frames Number of frames
     */
	AbstractPageReplacement(std::vector<int>& ref_string, int num_pages, int num_frames)
	{
		// Set and clean the ref string
        ref_string_ = ref_string;
        AbstractPageReplacement::CleanRefString(ref_string_);

		// Set the number of pages and frames
        num_pages_ = num_pages;
        num_frames_ = num_frames;
    }

    /*!
     * \brief calculate_page_faults is a virtual function
     * for calculating the page faults. This needs
     * to be overwritten in each class that inherets
     * \return 0
     */
    virtual int CalculatePageFaults() = 0;

    /*!
     * \brief generate_ref_string generates a random reference string
     * to be used in the page replacement algorithm. This is a random
     * array of ints where no two consecutive numbers are equal.
     *
     * \param size Size of the reference string to generate
     * \param upper_bound Upper bound of the reference string
     * \return Randomly filled vector with no equivelent sequential elements
     */
    static std::vector<int> GenerateRefString(int size, int upper_bound)
	{
		// Instantiate a new reference string a paramaterized size
		std::vector<int> ref_string(size);

		// If the user requested size zero then just return the array
		// this is mostly a sanity check so this function doesn't 
		// error out
		if (size == 0) {
			return ref_string;
		}

		// Set the 0th element as random outside the loop so the i-1th
		// element can be checked all through the loop
		ref_string[0] = rand() % upper_bound;

		// Loop from 1 to requested size
		for (int i = 1; i < size; ++i)
		{
			// Append a random number to the array
			ref_string[i] = rand() % upper_bound;

			// Loop while (and if) that just appended number is equal
			// to the last element
			while (ref_string[i] == ref_string[i - 1]) 
			{
				// Re-add a new random number to the array
				ref_string[i] = rand() % upper_bound;
			}
		}

		// Return the refernece string. C++ now does funcky magic
		// to optimize this so it isn't a slow pass by value
		return ref_string;
	}

    /*!
     * \brief clean_ref_string ensures that no two consecutive elements
     * are equal. If two elements are equal the one closer to the end
     * of the array is removed. Cleaning is done in place.
     * \param ref_string Array to clean
     */
    static void CleanRefString(std::vector<int>& ref_string)
	{
		// Loop through the array backwards so we can remove an element
		// and not mess up the indexing
		for (auto i = ref_string.end(); i != ref_string.begin(); --i) { 
			// If this element is equal to the one behind then remove
			// the current element
			if (*i == *(i - 1)) {
				ref_string.erase(i);
			}
	    } 
	}

    /*!
     * \brief FindInContainer takes a container that has begin
     * and end functions of type T and finds the needle of type int
     * in that container
     * \param needle Element to query for
     * \param haystack Deque to query in
     * \return True if element was found else False
     */
    template <class T>
    static bool FindInContainer(int needle, T haystack) {
        // Loop through the entire container
        for (auto i = haystack.begin(); i != haystack.end(); ++i)
        {
            // If the value of the current iterator is the
            // same as the needle return true.
            if (*i == needle) {
                return true;
            }
        }

        // If the whole deque has been queried with no results then the
        // needle is not in the haystack
        return false;
    }



protected:
    // Reference String
    std::vector<int> ref_string_;
    // Number of pages in the system
    int num_pages_;
    // Number of frames in the system
    int num_frames_;
};

/*!
 * \brief The FIFOPageReplacement class is used to calculate
 * the number of page faults in a system using the FIFO memory
 * swapping algorithm
 */
class FIFOPageReplacement: public AbstractPageReplacement
{
public:
    /*!
     * \brief FIFOPageReplacement constructs a FIFOPageReplacement
     * object with a the given values. This just calls the super constructor
     * in AbstractPageReplacement.
     * \param ref_string Ordered string of frame requests
     * \param num_pages Number of pages in the system
     * \param num_frames Number of frames in the system
     */
	FIFOPageReplacement(std::vector<int>& ref_string, int num_pages, int num_frames) 
	:AbstractPageReplacement(ref_string, num_pages, num_frames) {}

    /*!
     * \brief calculate_page_faults calculates the number of page faults using
     * the FIFO algorithm. This algorithm swaps out pages in the main memory
     * by keeping track of whichever memory was swapped in first.
     * \return The number of page faults calculated when using this algorithm
     */
    int CalculatePageFaults()
	{
		// Initialize a deque to store the pages that are in memory. Using a 
        // deque instead of a queue because it is easier to search through
        // because you can use iterators
        std::deque <int> current_pages;

		// Start the page_fault count
        int page_faults = 0;

        // Iterate through ths memory requests list
        for (std::vector<int>::iterator i = ref_string_.begin(); i != ref_string_.end(); ++i)
		{
            // If the currently requested page is not in memory then
            // it needs to be swapped in
            if (!FindInContainer<std::deque<int>>(*i, current_pages))
			{
                // If the current pages is at max capacity then a page needs
                // to be swapped out before another can be swapped in
                if (current_pages.size() == (size_t) num_frames_)
				{
                    // Remove the page from the top of the queue to make
                    // room. This is the FIFO part of the algorithm.
                    current_pages.pop_front();
				}

                // Add the currently requestsed page onto the back of the queue
                current_pages.push_back(*i);

				// Add a page fault
                page_faults += 1;
			} 
		}

        // Return the total page fault cound
        return page_faults;
	}

};

class LRUPageReplacement: public AbstractPageReplacement
{
public:
    /*!
     * \brief LRUPageReplacement constructs a LRUPageReplacement
     * object with a the given values. This just calls the super constructor
     * in AbstractPageReplacement.
     * \param ref_string Ordered string of frame requests
     * \param num_pages Number of pages in the system
     * \param num_frames Number of frames in the system
     */
	LRUPageReplacement(std::vector<int>& ref_string, int num_pages, int num_frames) 
	:AbstractPageReplacement(ref_string, num_pages, num_frames) {}

    /*!
     * \brief CalculatePageFaults calculates the number of page faults using
     * the LRU algorithm. This algorithm swaps out pages in the main memory
     * by keeping track of the Least Recently Used page in current memory
     * \return The number of page faults calculated when using this algorithm
     */
    int CalculatePageFaults()
	{
		// Initialize a deque to store the pages that are in memory. Using a 
		// deque instead of a queue because it is easier to search through a 
		// deque because you can use iterators
        std::deque <int> current_pages;

		// Start the page_fault count
		int page_faults = 0;

        // Iterate through the memory requests
        for (auto i = ref_string_.begin(); i != ref_string_.end(); ++i)
		{
            // If the currently requested page is not in memory then
            // that's a page fault
            if (!FindInContainer<std::deque<int>>(*i, current_pages))
            {
                // If current pages in the main memory is at
                // maximum size then remove the front element
                if (current_pages.size() == (size_t) num_frames_)
				{
                    // Remove the top of the queue to make room
                    current_pages.pop_front();
				}

				// Add this page onto the back of the queue
                current_pages.push_back(*i);

				// Add a page fault
				page_faults += 1;
			}
            // If the currently requested page IS in memory then its' position
            // needs to be reset. This current algorithm must not be the most efficient
            // way to do this. This is an O(n^2) algorithm at current. Maybe through use
            // of an unordered set (which can search in O(1) time) and a hash table for
            // storing the order we can achieve better time complexity. I think if this
            // were to be implemented we could implement a separate container class
            // for each of the algorithms. TODO.
			else 
			{
                // Iterate through all of the pages currently in memory
                for (auto j = current_pages.begin(); j != current_pages.end(); ++j)
				{
                    // If the currently requested page is in memory then delete
                    // its current position and add it on to the back of the array
                    // so it will not be chosen as the LRU
					if (*j == *i)
					{
                        current_pages.push_back(*j);
                        current_pages.erase(j);
					}
				}
			}
		}

        // Return the number of page faults
		return page_faults;
	}

};

class OPTPageReplacement: public AbstractPageReplacement
{
public:
	OPTPageReplacement(std::vector<int>& ref_string, int num_pages, int num_frames) 
	:AbstractPageReplacement(ref_string, num_pages, num_frames) {}

    int CalculatePageFaults()
	{
		// Initialize a vector to store the memory_requests
        std::vector <int> current_pages;

		// Start the page_fault count
		int page_faults = 0;


        for (auto i = ref_string_.begin(); i != ref_string_.end(); ++i)
		{
            // If the memory_requests is at maximum capacity AND it cannot be found then
            // we have to shift the memory around. However if either one of those is false
            // then we can just add it to the memory
            if (current_pages.size() == (size_t) num_pages_ &&
                !FindInContainer<std::vector<int>>(*i, current_pages))
            {
                // If the page that is being requested is not currently in memory
                // then we need to find the page currently in memory that is furthest
                // away from being requested again. We accomplish this using a vector
                // with some extra fluff around accessing but this could be easily
                // abstracted away to another class
                //      1. An object within the set cannot be added again
                //      2. The set will remain ordered
                // These are both integral to how we are going to use the data structure.
                // Conceptually we are using the set like a unique stack. Meaning that we append
                // a value onto the top only if that element is not already in the stack. Then we can
                // remove the page from the top of the stack (or any page that hasn't shown up
                // in the stack if the stack's length is less than the frame length.

                // Initialize the unique stack
                std::vector<int> uniqueMemoryStack;

                // Loop through the rest of the memory adding each element to the stack
                // if it is currently in memory.
                for (auto j = i; j != ref_string_.end(); j++)
                {
                    // If the memory request isn't currently in memory then just continue
                    if(!FindInContainer<std::vector<int>>(*j, current_pages))
                    {
                        continue;
                    }

                    // If the memory request is in memory then add it to the unique stack
                    if(!FindInContainer<std::vector<int>>(*j, uniqueMemoryStack))
                    {
                        uniqueMemoryStack.push_back(*j);
                    }
                }

                // Now there are two cases that we have to check to find the memory frame
                // that is the furthest away from being used. If the size of the stack
                // is equal to the number of pages then we can just pop the top of the stack
                // and remove that item from the array
                if (uniqueMemoryStack.size() == (size_t) num_pages_)
                {
                    // remove *uniqueMemoryStack.rbegin();
                    for (auto j = current_pages.begin(); j != current_pages.end(); j++)
                    {
                        if (*j == *uniqueMemoryStack.rbegin())
                        {
                            current_pages.erase(j);
                            break;
                        }
                    }
                }
                // If the length of the memory stack isnt equal to the number of pages then
                // there exists a page in memory that isn't used in the rest of the running
                // of the program. It makes sense to check and remove all elements that do
                // not show up in this list. However even if we do that it will still cause
                // a page fault when we try to access memory that isn't in the current pages
                // To remove everything that isn't in the stack we iterate through the stack
                // and replace every entry in the current pages with the entries in the stack
                else
                {
                    for (auto j = current_pages.begin(); j != current_pages.end(); j++)
                    {
                        // If the page was not found in the memory stack then remove it
                        if (!FindInContainer<std::vector<int>>(*j, uniqueMemoryStack))
                        {
                            current_pages.erase(j);
                            break;
                        }
                    }
                }


                current_pages.push_back(*i);
                page_faults += 1;
            }

            // If the current pages in memory is not equal to the maximum number of pages that
            // can be contained in memory then the it is a page fault but it is handled differently
            // We can just add to the page fault count and push the page into memory. If this
            // algorithm was going to be used in a prod setting then these would be factored
            // out so that the conditions were only checked once but I am leaving them here
            // for readability.
            else if (current_pages.size() != (size_t) num_frames_ &&
                     !FindInContainer<std::vector<int>>(*i, current_pages))
            {
                page_faults += 1;
                current_pages.push_back(*i);
            }
		}

		return page_faults;
	}
};
