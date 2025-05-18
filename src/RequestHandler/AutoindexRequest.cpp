/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AutoindexRequest.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 16:38:06 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/18 10:01:31 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"

void	RequestHandler::handleDirectoryListing(Client &client)
{
	std::string path = client.getRequest().getPath();
	std::string uri = client.getRequest().getUri();
	if (path[path.length() - 1] != '/')
		path += "/";
	if (uri[uri.length() - 1] != '/')
		uri += "/";
	// std::string root = client.getServerConf()->getRoot();
	// if (client.getServerConf() && !client.getLocationConf()->getLocRoot().empty())
	// 	root = client.getLocationConf()->getLocRoot();
	DIR	*dirp = opendir(path.c_str());

	LOG_DEBUG("About to list " + uri);
	if (!dirp) {
		client.sendErrorResponse(500, ""); //Internal Server error
		return;
	}
	//Build HTML Content: check with telnet - nginx what exactly to build
	struct dirent				*dirent;
	struct stat					fileStat;
	std::vector<std::string>	dirs;
	std::vector<std::string>	files;
	
	//write the HTML content: d_name etc
	std::stringstream html;

	html << "<!DOCTYPE html>\n<html>\n<head>\n";
	html << "<title>Index of " << uri << "</title>\n";
	html << "<style>table { width: 100%; } th, td { text-align: left; padding: 8px; } "
		 << "tr:nth-child(even) { background-color: #f2f2f2; }</style>\n";
	html << "</head>\n<body>\n";
	html << "<h1>Index of " << uri << "</h1>\n";
	html << "<table>\n<tr><th>Name</th><th>Last Modified</th><th>Size</th></tr>\n";
	// Add parent directory link unless we're at root
	if (uri != "/")
		html << "<tr><td><a href=\"../\">Parent Directory</a></td><td>-</td><td>-</td></tr>\n";
	else
		html << "<tr><td><a href=\"./\">Parent Directory</a></td><td>-</td><td>-</td></tr>\n";

	while ((dirent = readdir(dirp)))
	{
		std::cout << "dirent reading " << dirent->d_name << std::endl;
		std::string name = dirent->d_name;
		// Skip hidden files and . and ..
		if (name[0] == '.') {
			continue;
		}
		std::string	fullPath = path + name;
		// skip if an error occur in stat
		if(stat(fullPath.c_str(), &fileStat) < 0){
			continue;
		}
		if (S_ISDIR(fileStat.st_mode)){
			dirs.push_back(name);
		}
		else{
			files.push_back(name);
		}
	}
	if (errno) // will catch errors from readdir
	{
		LOG_ERR("Readdir error: " + std::string(strerror(errno)));
		//handle error
	}
	std::cout << "REDIR LIST DONE" << std::endl;
	std::sort(dirs.begin(), dirs.end());
	std::cout << "DIR SORT DONE" << std::endl;
	std::sort(files.begin(), files.end());
	std::cout << "FILES SORT DONE" << std::endl;

	for (const std::string& name : dirs) {
		std::string fullPath = path + name;
		std::string fullUri = uri + name;
		if (stat(fullPath.c_str(), &fileStat) < 0) continue;
		
		// Format time
		char timeStr[80];
		struct tm* timeinfo = localtime(&fileStat.st_mtime);
		strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", timeinfo);
		
		html << "<tr><td><a href=\"" << fullUri << "/\">" << name << "/</a></td>";
		html << "<td>" << timeStr << "</td>";
		html << "<td>-</td></tr>\n";
	}

	// Add files to HTML
	for (const std::string& name : files) {
		std::string fullPath = path + name;
		std::string fullUri = uri + name;
		if (stat(fullPath.c_str(), &fileStat) < 0) continue;
		
		// Format time
		char timeStr[80];
		struct tm* timeinfo = localtime(&fileStat.st_mtime);
		strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", timeinfo);
		
		// Format file size
		std::string sizeStr;
		if (fileStat.st_size < 1024)
			sizeStr = std::to_string(fileStat.st_size) + " B";
		else if (fileStat.st_size < 1024 * 1024)
			sizeStr = std::to_string(fileStat.st_size / 1024) + " KB";
		else
			sizeStr = std::to_string(fileStat.st_size / (1024 * 1024)) + " MB";
		
		html << "<tr><td><a href=\"" << fullUri << "\">" << name << "</a></td>";
		html << "<td>" << timeStr << "</td>";
		html << "<td>" << sizeStr << "</td></tr>\n";
	}

	html << "</table>\n</body>\n</html>";
	closedir(dirp);

	std::string html_str = html.str();

	// Send response to client
	client.getResponse().setStatusCode(200);
	client.getResponse().setHeaderField("Content-Type", "text/html");
	client.getResponse().setHeaderField("Content-Length", std::to_string(html_str.size()));
	std::vector<char> html_vector(html_str.begin(), html_str.end());
	client.getResponse().appendBodyBuffer(html_vector, html_str.size(), true);
	client.getResponse().setState(READ);
	client.setState(SENDING_RESPONSE);
	client.getTracker().setResponseStart();
}
