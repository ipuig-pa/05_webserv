/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler copy 2.cpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 16:38:06 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/03 10:54:08 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"

void	RequestHandler::handleDirectoryListing(Client &client)
{
	std::string path = client.getRequest().getPath();
	DIR	*dirp = opendir(path.c_str());

	LOG_DEBUG("About to list " + path);
	if (!dirp)
	{
		client.prepareErrorResponse(500); //Internal Server error
		return;
	}
	//Build HTML Content: check with telnet - nginx what exactly to build
	struct dirent				*dirent;
	struct stat					fileStat;
	std::vector<std::string>	dirs;
	std::vector<std::string>	files;
	
	//write the HTML content: d_name etc
	std::stringstream html;

	//COMPARE AND CHECK THE CONTENT WITH NGINX OUTPUT!!!!! (USE TELNET)
	html << "<!DOCTYPE html>\n<html>\n<head>\n";
	html << "<title>Index of " << path << "</title>\n";
	html << "<style>table { width: 100%; } th, td { text-align: left; padding: 8px; } "
		 << "tr:nth-child(even) { background-color: #f2f2f2; }</style>\n";
	html << "</head>\n<body>\n";
	html << "<h1>Index of " << path << "</h1>\n";
	html << "<table>\n<tr><th>Name</th><th>Last Modified</th><th>Size</th></tr>\n";
	// Add parent directory link unless we're at root
	if (path != "/") {
		html << "<tr><td><a href=\"../\">Parent Directory</a></td><td>-</td><td>-</td></tr>\n";
	}

	while ((dirent = readdir(dirp)))
	{
		std::cout << "dirent reading" << dirent->d_name << std::endl;
		std::string name = dirent->d_name;
		// Skip hidden files and . and ..
		if (name[0] == '.' && (name != "..")) {
			continue;
		}
		std::string	fullPath = path + "/" + name;
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
		LOG_ERR("Poll error: " + std::string(strerror(errno)));
		//handle error
	}
	std::sort(dirs.begin(), dirs.end());
	std::sort(files.begin(), files.end());

	for (const std::string& name : dirs) {
		std::string fullPath = path + "/" + name;
		if (stat(fullPath.c_str(), &fileStat) < 0) continue;
		
		// Format time
		char timeStr[80];
		struct tm* timeinfo = localtime(&fileStat.st_mtime);
		strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", timeinfo);
		
		html << "<tr><td><a href=\"" << name << "/\">" << name << "/</a></td>";
		html << "<td>" << timeStr << "</td>";
		html << "<td>-</td></tr>\n";
	}
	
	// Add files to HTML
	for (const std::string& name : files) {
		std::string fullPath = path + "/" + name;
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
		
		html << "<tr><td><a href=\"" << name << "\">" << name << "</a></td>";
		html << "<td>" << timeStr << "</td>";
		html << "<td>" << sizeStr << "</td></tr>\n";
	}
	
	html << "</table>\n</body>\n</html>";
	closedir(dirp);

	// Send response to client
	client.getResponse().setStatusCode(200);
	client.getResponse().setHeaderField("Content-Type", "text/html");
	client.getResponse().setHeaderField("Content-Length", std::to_string(html.str().size()));
	client.getResponse().setBodyBuffer(html.str());
	client.setState(SENDING_RESPONSE);
}
