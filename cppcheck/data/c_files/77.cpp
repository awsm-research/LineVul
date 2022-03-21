static ssize_t driver_override_show(struct device *dev,
struct device_attribute *attr, char *buf)
{
struct platform_device *pdev = to_platform_device(dev);

	return sprintf(buf, "%s\n", pdev->driver_override);
}
