schema "public" {}

table "device" {
  schema = schema.public

  column "id" {
    type = text
    null = false
  }
  unique "device_id" {
    columns = [column.id]
  }

  column "class" {
    type = text
    null = false
  }
  check {
    expr = "class IN ('SENSOR', 'JAMMER')"
  }

  column "fingerprint" {
    type = text
    null = false
  }

  primary_key {
    columns = [column.class, column.fingerprint]
  }
}

table "device_position" {
  schema = schema.public

  column "device_id" {
    type = text
    null = false
  }

  column "mode" {
    type = text
    null = false
  }
  check {
    expr = "mode IN ('AUTO', 'MANUAL', 'ALWAYS_MANUAL')"
  }

  column "latitude" {
    type = real
  }

  column "longitude" {
    type = real
  }

  column "altitude" {
    type = real
  }

  column "azimuth" {
    type = real
  }

  primary_key {
    columns = [column.device_id]
  }

  foreign_key "device_position_device_id" {
    columns     = [column.device_id]
    ref_columns = [table.device.column.id]
  }
}


table "device_settings" {
  schema = schema.public

  column "device_id" {
    type = text
    null = false
  }

  column "disabled" {
    type = boolean
    null = false
  }

    primary_key {
    columns = [column.device_id]
  } 

  foreign_key "device_settings_device_id" {
    columns     = [column.device_id]
    ref_columns = [table.device.column.id]
  }
}

table "jammer_timeout_status" {
  schema = schema.public

  column "device_id" {
    type = text
    null = false
  }

  column "started_at" {
    type = timestamp 
    null = false
  }

  column "timeout" {
    type = integer 
    null = false
  }

  primary_key {
    columns = [column.device_id]
  }

  foreign_key "jammer_timeout_status_device_id" {
    columns     = [column.device_id]
    ref_columns = [table.device.column.id]
    on_delete = CASCADE
  }
}

table "jammer_bands_active" {
  schema = schema.public

  column "device_id" {
    type = text
    null = false
  }

  column "band" {
    type = text 
    null = false
  }

  foreign_key "jammer_bands_active_device_id" {
    columns     = [column.device_id]
    ref_columns = [table.jammer_timeout_status.column.device_id]
    on_delete = CASCADE
  }
}

table "jammer_mode" {
  schema = schema.public

  column "sensor_id" {
    type = text
    null = false
  }

  column "mode" {
    type = text
    null = false
  }
  check { 
    expr = "mode IN ('AUTO', 'MANUAL')" 
  }

  column "timeout" {
    type = integer 
    null = false
  }

  primary_key {
    columns = [column.sensor_id]
  }

  foreign_key "jammer_mode_sensor_id" {
    columns     = [column.sensor_id]
    ref_columns = [table.device.column.id]
    on_delete = CASCADE
  }
}


