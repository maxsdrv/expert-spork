#!/bin/sh

atlas migrate apply \
  --dir "file://migrations" \
  --url postgres://$DB_PG_USER:$DB_PG_PASS@$DB_PG_HOST:$DB_PG_PORT/$DB_PG_NAME?sslmode=disable
