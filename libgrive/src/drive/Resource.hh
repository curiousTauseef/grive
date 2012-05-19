/*
	grive: an GPL program to sync a local directory with Google Drive
	Copyright (C) 2012  Wan Wai Ho

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation version 2
	of the License.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#pragma once

#include "Entry.hh"
#include "http/Agent.hh"
#include "util/Exception.hh"
#include "util/FileSystem.hh"

#include <string>
#include <vector>
#include <iosfwd>

namespace gr {

/*!	\brief	A resource can be a file or a folder in the google drive

	The google drive contains a number of resources, which is represented by this class.
	It also contains linkage to other resources, such as parent and childrens.
*/
class Resource
{
public :
	struct Error : virtual Exception {} ;

public :
	explicit Resource( const xml::Node& entry ) ;
	explicit Resource( const Entry& entry, Resource *parent = 0 ) ;
	Resource(
		const std::string& name,
		const std::string& kind,
		const std::string& href ) ;
	void Swap( Resource& coll ) ;
	
	// default copy ctor & op= are fine

	bool IsFolder() const ;

	std::string Name() const ;
	std::string SelfHref() const ;
	std::string ResourceID() const ;
	std::string ParentHref() const ;
	
	const Resource* Parent() const ;
	Resource* Parent() ;
	void AddChild( Resource *child ) ;
	Resource* FindChild( const std::string& title ) ;
	
	fs::path Path() const ;
	bool IsInRootTree() const ;

	void FromRemote( const Entry& e ) ;
	void Update( http::Agent *http, const http::Headers& auth ) ;
	void Delete( http::Agent* http, const http::Headers& auth ) ;

private :
	/// State of the resource. indicating what to do with the resource
	enum State
	{
		/// The best state: the file is the same in google drive and in local.
		sync,
		
		/// Resource created in local, but google drive does not have it.
		/// We should create the resource in google drive and upload new content
		new_local,
		
		/// Resource created in google drive, but not exist in local.
		/// We should download the file.
		new_remote
	} ;
	
private :
	void Download( http::Agent* http, const fs::path& file, const http::Headers& auth ) const ;
	bool Upload( http::Agent* http, std::streambuf *file, const http::Headers& auth ) ;

private :
	Entry					m_entry ;
	
	// not owned
	Resource				*m_parent ;
	std::vector<Resource*>	m_child ;
	
	State					m_state ;
} ;

} // end of namespace

namespace std
{
	void swap( gr::Resource& c1, gr::Resource& c2 ) ;
}
