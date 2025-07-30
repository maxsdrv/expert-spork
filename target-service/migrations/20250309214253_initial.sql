-- Create "device" table
CREATE TABLE "device" (
  "id" text NOT NULL,
  "class" text NOT NULL,
  "fingerprint" text NOT NULL,
  PRIMARY KEY ("class", "fingerprint"),
  CONSTRAINT "device_id" UNIQUE ("id"),
  CONSTRAINT "device_class_check" CHECK (class = ANY (ARRAY['SENSOR'::text, 'JAMMER'::text]))
);
-- Create "device_position" table
CREATE TABLE "device_position" (
  "device_id" text NOT NULL,
  "mode" text NOT NULL,
  "latitude" real NOT NULL,
  "longitude" real NOT NULL,
  "altitude" real NOT NULL,
  "azimuth" real NOT NULL,
  PRIMARY KEY ("device_id"),
  CONSTRAINT "device_position_device_id" FOREIGN KEY ("device_id") REFERENCES "device" ("id") ON UPDATE NO ACTION ON DELETE NO ACTION,
  CONSTRAINT "device_position_mode_check" CHECK (mode = ANY (ARRAY['AUTO'::text, 'MANUAL'::text, 'ALWAYS_MANUAL'::text]))
);
-- Create "device_settings" table
CREATE TABLE "device_settings" (
  "device_id" text NOT NULL,
  "disabled" boolean NOT NULL,
  PRIMARY KEY ("device_id"),
  CONSTRAINT "device_settings_device_id" FOREIGN KEY ("device_id") REFERENCES "device" ("id") ON UPDATE NO ACTION ON DELETE NO ACTION
);
-- Create "jammer_timeout_status" table
CREATE TABLE "jammer_timeout_status" (
  "device_id" text NOT NULL,
  "started_at" timestamp NOT NULL,
  "timeout" integer NOT NULL,
  PRIMARY KEY ("device_id"),
  CONSTRAINT "jammer_timeout_status_device_id" FOREIGN KEY ("device_id") REFERENCES "device" ("id") ON UPDATE NO ACTION ON DELETE CASCADE
);
-- Create "jammer_bands_active" table
CREATE TABLE "jammer_bands_active" (
  "device_id" text NOT NULL,
  "band" text NOT NULL,
  CONSTRAINT "jammer_bands_active_device_id" FOREIGN KEY ("device_id") REFERENCES "jammer_timeout_status" ("device_id") ON UPDATE NO ACTION ON DELETE CASCADE
);
-- Create "jammer_mode" table
CREATE TABLE "jammer_mode" (
  "sensor_id" text NOT NULL,
  "mode" text NOT NULL,
  "timeout" integer NOT NULL,
  PRIMARY KEY ("sensor_id"),
  CONSTRAINT "jammer_mode_sensor_id" FOREIGN KEY ("sensor_id") REFERENCES "device" ("id") ON UPDATE NO ACTION ON DELETE CASCADE,
  CONSTRAINT "jammer_mode_mode_check" CHECK (mode = ANY (ARRAY['AUTO'::text, 'MANUAL'::text]))
);
