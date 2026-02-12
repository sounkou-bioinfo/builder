# Test that the builder binary exists and is executable
expect_true(file.exists(builder::builder_path()))

# Test version output
v <- builder::builder_version()
expect_true(grepl("Builder", v))

# Test help (exit code 0)
rc <- builder::builder(args = "-help", stdout = FALSE, stderr = FALSE)
expect_equal(rc, 0L)

# Test -version flag (exit code 0)
rc <- builder::builder(args = "-version", stdout = FALSE, stderr = FALSE)
expect_equal(rc, 0L)
