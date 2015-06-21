require "image_optim"
require 'yaml'

module Jekyll

    class ImageOptimGenerator < Generator
        safe true

        def generate(site)
	  puts 'Optimizing Images'
	  settings_new = {}
	  settings = {}

    # Does file exists?
    if File.file?("images/image_optim_cache.yml")
	  	settings = YAML::load_file "images/image_optim_cache.yml"
	  end

	  image_optim = ImageOptim.new(:pngout => false, :svgo => false, :verbose => false)
    Dir.glob('images/**/*.{gif,jpeg,jpg,png}') do |rb_file|
      # Get the file modification time
      cmtime = File.mtime(rb_file)

      if settings.has_key?(rb_file)
        # Is modified?
        if settings[rb_file] != cmtime
          image_optim.optimize_image!(rb_file)
          puts "Optimized: #{rb_file}"
          thash = {}
          thash[rb_file] = cmtime
          settings.merge!(thash)
        end
      else
        image_optim.optimize_image!(rb_file)
        puts "Optimized: #{rb_file}"
        thash = {}
        thash[rb_file] = cmtime
        settings.merge!(thash)
      end
    end

    # Write modification hash to file
    File.open("images/image_optim_cache.yml", "w") do |file|
      file.write settings.to_yaml
    end
	end
    end
end
