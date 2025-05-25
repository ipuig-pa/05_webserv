/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AutoindexRequest.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipuig-pa <ipuig-pa@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 16:38:06 by ipuig-pa          #+#    #+#             */
/*   Updated: 2025/05/25 10:34:48 by ipuig-pa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"

/*-------------METHODS--------------------------------------------------------*/

void	RequestHandler::_handleDirectoryListing(Client &client)
{
	std::string path = client.getRequest().getPath();
	std::string uri = client.getRequest().getUri();
	if (path[path.length() - 1] != '/')
		path += "/";
	if (uri[uri.length() - 1] != '/')
		uri += "/";
	DIR	*dirp = opendir(path.c_str());

	LOG_DEBUG("About to list " + uri);
	if (!dirp) {
		client.sendErrorResponse(500, ""); //Internal Server error
		return;
	}
	struct dirent				*dirent;
	struct stat					fileStat;
	std::vector<std::string>	dirs;
	std::vector<std::string>	files;

	std::stringstream html;

	html << "<!DOCTYPE html>\n<html>\n<head>\n";
	html << "<title>Index of " << uri << "</title>\n";
	html << "<style>table { width: 100%; } th, td { text-align: left; padding: 8px; } "
		 << "tr:nth-child(even) { background-color: #f2f2f2; }</style>\n";
	html << "</head>\n<body>\n";
	html << "<h1>Index of " << uri << "</h1>\n";
	html << "<table>\n<tr><th>Name</th><th>Last Modified</th><th>Size</th></tr>\n";

	if (uri != "/")
		html << "<tr><td><a href=\"../\">Parent Directory</a></td><td>-</td><td>-</td></tr>\n";
	else
		html << "<tr><td><a href=\"./\">Parent Directory</a></td><td>-</td><td>-</td></tr>\n";

	while ((dirent = readdir(dirp)))
	{
		std::string name = dirent->d_name;
		if (name[0] == '.') {
			continue;
		}
		std::string	fullPath = path + name;
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
		client.sendErrorResponse(500, "");
	}
	std::sort(dirs.begin(), dirs.end());
	std::sort(files.begin(), files.end());

	for (const std::string& name : dirs) {
		std::string fullPath = path + name;
		std::string fullUri = uri + name;
		if (stat(fullPath.c_str(), &fileStat) < 0)
			continue;
		char timeStr[80];
		struct tm* timeinfo = localtime(&fileStat.st_mtime);
		strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", timeinfo);
		
		html << "<tr><td><a href=\"" << fullUri << "/\">" << name << "/</a></td>";
		html << "<td>" << timeStr << "</td>";
		html << "<td>-</td></tr>\n";
	}

	for (const std::string& name : files) {
		std::string fullPath = path + name;
		std::string fullUri = uri + name;
		if (stat(fullPath.c_str(), &fileStat) < 0) 
			continue;
		char timeStr[80];
		struct tm* timeinfo = localtime(&fileStat.st_mtime);
		strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", timeinfo);
		
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

	client.getResponse().setStatusCode(200);
	client.getResponse().setHeaderField("Content-Type", "text/html");
	client.getResponse().setHeaderField("Content-Length", std::to_string(html_str.size()));
	std::vector<char> html_vector(html_str.begin(), html_str.end());
	client.getResponse().appendBodyBuffer(html_vector, html_str.size(), true);
	client.getResponse().setState(READ);
	client.setState(SENDING_RESPONSE);
	client.getTracker().setResponseStart();
}
