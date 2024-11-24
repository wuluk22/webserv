/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Brain.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clegros <clegros@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/21 08:33:48 by clegros           #+#    #+#             */
/*   Updated: 2024/09/21 08:33:50 by clegros          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BRAIN_HPP
# define BRAIN_HPP
# include <iostream>
# include <string>

class Brain
{
	private:
		std::string		ideas[100];
		unsigned int	size;
	public:
		Brain();
		Brain(const Brain& rhs);
		~Brain();

		Brain&				operator=(const Brain& rhs);
		void				addIdea(std::string idea);
		const std::string&	getIdea(unsigned int index) const;
};

#end