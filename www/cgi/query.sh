#!/bin/bash

echo "Content-Type: text/plain"
echo ""

QUERY="$QUERY_STRING"

echo "Query string from request uri is: $QUERY"

IFS='&' read -ra PARAMS <<< "$QUERY"
for param in "${PARAMS[@]}"; do
	key="${param%%=*}"
	value="${param#*=}"
	value="$(printf '%b' "${value//%/\\x}")"
	echo "$key = $value"
done

# cmd: curl "http://localhost:8080/www/cgi/query.sh?TASK=TesingQuery&Status=200OK"